#include <iostream>

// stb_image要求定义宏STB_IMAGE_IMPLEMENTATION才能用
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "core.h"

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"
// 3-Application中的Application.h
#include "Application.h"
#include "shader.h"

GLuint VAO;
// 纹理对象
GLuint texture;
// 封装的着色器程序对象
Shader* shader = nullptr;

// 窗口尺寸变化的回调
void framebufferSizeCallback(const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
    // 使用glad中的glViewport来动态更新视口的大小
    glViewport(0, 0, width, height);
}

// 键盘输入的回调
/*
 * key: 字母按键码 scancode: 物理按键码(不常用) action: 按键动作(按下/抬起) mods: 修饰键(shift, ctrl, alt)
 */
void keyCallback(const int key, int scancode, const int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "key pressed: " << key << std::endl;
    }
}

// 定义和编译着色器
void prepareShader() {
    shader = new Shader(
        "assets/shader/vertex.glsl",
        "assets/shader/fragment.glsl"
        );
}

// 准备包含uv坐标的缓冲区
void prepareEBOBuffer() {
    float positions[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
    };
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };
    // 纹理坐标数据
    float uvs[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.5f, 1.0f,
    };

    // 顶点索引顺序数据, 方便复用顶点
    unsigned int indices[] = {
        0, 1, 2,
    };

    GLuint position, color, EBO, uv;
    // VBO
    GL_CALL(glGenBuffers(1, &position));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, position));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

    GL_CALL(glGenBuffers(1, &color));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

    GL_CALL(glGenBuffers(1, &uv));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, uv));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW));

    // 创建EBO. 注意target是GL_ELEMENT_ARRAY_BUFFER
    GL_CALL(glGenBuffers(1, &EBO));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // VAO
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glBindVertexArray(VAO));

    // 绑定VBO, EBO, 加入属性描述信息
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, position));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));
    // uv坐标属性
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, uv));
    GL_CALL(glEnableVertexAttribArray(2));
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr));

    // 绑定EBO, 📌📌这一行不可以省略
    // 📌📌因为glVertexAttribPointer内会自动查询并绑定当前的VBO, 但不会查询EBO
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    // 解绑VAO
    GL_CALL(glBindVertexArray(0));
}

// 纹理加载
void prepareTexture() {
    // 1. stbImage 读取图片文件
    int width, height, channels;
    // 反转Y轴(因为OpenGL纹理的坐标系是左下角为原点, 而图片文件的坐标系是左上角为原点)
    stbi_set_flip_vertically_on_load(true);
    // 读取图片文件, 并且全部转换为RGBA格式
    unsigned char* data = stbi_load("assets/texture/reisen.jpg", &width, &height, &channels, STBI_rgb_alpha);

    // 2. 创建并绑定纹理对象
    glGenTextures(1, &texture);
    // 激活纹理单元0 (虽然默认情况下也会激活0) (默认一共有0-15, 共16个纹理单元)
    glActiveTexture(GL_TEXTURE0);
    // 绑定纹理对象 (到gl状态机的GL_TEXTURE_2D插槽)
    // 📌📌同时还会将纹理对象自动绑定到当前激活的纹理单元上
    glBindTexture(GL_TEXTURE_2D, texture);

    // 3. 传输纹理数据到GPU (会开辟GPU内存)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // 📌别忘了释放图片数据(已经传输到GPU, 内存中就不需要了)
    stbi_image_free(data);

    // 4. 设置纹理参数
    // 纹理过滤方式. 图片被放大时采用插值, 缩小时就不插值(取临近点像素)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 纹理环绕方式
    // 纹理坐标超出范围时, 采用重复的方式(也是默认方式)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

// 执行渲染操作
void render() {
    // 画布清理操作也算渲染操作
    // 执行画布清理操作(用glClearColor设置的颜色来清理(填充)画布)
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();

    // 通过uniform将采样器绑定到0号纹理单元上
    shader->setInt("sampler", 0);

    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // 使用EBO顶点索引绘制. 加载了EBO后indices参数表示EBO内偏移量
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    shader->end();
}

/*
 * 简单的2D纹理贴图
 *  1. 创建纹理对象
 *       -读取图片文件(stbImage)
 *       -创建纹理对象
 *       -绑定纹理单元(激活纹理单元+绑定纹理对象)
 *       -传输纹理数据到GPU
 *  2. 设置纹理参数 (纹理过滤方式, 纹理环绕方式)
 *  3. 设置采样器, uv坐标
 *       -uv坐标属性需要传递给VBO, 于是还要相应地设置VAO
 *       -着色器内也需要读取uv坐标以及设置采样器
 *       -将采样器与纹理单元绑定: glUniform1i(采样器, 纹理单元)
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "着色器API的封装")) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置事件回调
    // 窗体尺寸变化
    APP->setOnResizeCallback(framebufferSizeCallback);
    // 键盘输入
    APP->setOnKeyboardCallback(keyCallback);

    // 设置擦除画面时的颜色. (擦除画面其实就是以另一种颜色覆盖当前画面)
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    // 编译着色器
    prepareShader();
    // 初始化VBO等资源
    prepareEBOBuffer();
    // 加载纹理
    prepareTexture();

    // 3. 执行窗体循环. 📌📌每次循环为一帧
    // 窗体只要保持打开, 就会一直循环
    while (APP->update()) {
        // 渲染操作
        render();
    }

    // 4. 清理和关闭
    APP->destroy();
    return 0;
}
#include <iostream>

#include "core.h"

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"
// 3-Application中的Application.h
#include "Application.h"
#include "shader.h"
#include "TextureMipMap.h"

GLuint VAO;
// 封装的着色器程序对象
Shader* shader = nullptr;
// 纹理对象
TextureMipMap* texture = nullptr;
// 当前的变换矩阵. (构造函数传递1.0f会初始化为单位矩阵)
glm::mat4 transform(1.0f);

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

// 绕z轴旋转45度
void rotate() {
    /*
     * glm::rotate用来生成旋转矩阵
     * arg1: 当前变换矩阵. 会将生成的变换矩阵右乘到当前变化矩阵 ==> 📌📌注意矩阵*向量的乘法顺序. 越靠右变换的越先执行
     * arg2: 旋转角度(弧度制, 需要用glm::radians转换)
     * arg3: 旋转轴(单位向量)
     */
    transform = glm::rotate(transform,glm::radians(45.0f),glm::vec3(0.0f, 0.0f, 1.0f));
}

// 平移
void translate() {
    /*
     * glm::translate用来生成平移矩阵
     * arg1: 当前变换矩阵
     * arg2: 平移向量
     */
    transform = glm::translate(transform,glm::vec3(0.5f, 0.5f, 0.0f));
}

// 缩放
void scale() {
    /*
     * glm::scale用来生成缩放矩阵
     * arg1: 当前变换矩阵
     * arg2: 各个轴上的缩放因子
     */
    transform = glm::scale(transform,glm::vec3(0.5f, 0.5f, 1.0f));
}

void doTransform() {
    rotate();
    translate();
    // scale();
}

// 定义和编译着色器
void prepareShader() {
    shader = new Shader(
        "assets/shader/default/vertex.glsl",
        "assets/shader/default/fragment.glsl"
        );
}

// 准备包含uv坐标的缓冲区
void prepareEBOBuffer() {
    float positions[] = {
        -0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
    };
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f,
    };
    // 纹理坐标数据
    float uvs[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
    };

    // 顶点索引顺序数据, 方便复用顶点
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
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
    texture = new TextureMipMap("assets/texture/reisen.jpg", 0);
    texture->bindTexture();
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
    // -> 让采样器知道要采样哪个纹理单元
    shader->setInt("sampler", 0);
    shader->setMat4("transform", transform);

    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // 使用EBO顶点索引绘制. 加载了EBO后indices参数表示EBO内偏移量
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    Shader::end();
}

/**
 * GLM数学库实现基本空间变换(平移, 旋转, 缩放)
 *  - 引入glm中的一系列头文件. (因为很多地方需要用到, 所以在10-ShaderClass/GLconfig/core.h中包含)
 *  - 顶点着色器内加入uniform变换矩阵, 着色器类中加入设置变换矩阵的方法
 *  - 进行连续复合变换时, 📌📌注意矩阵乘向量的乘法顺序. p'=ABCp, 则变换顺序为CBA
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "GLM数学库实现基本空间变换")) {
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


    doTransform();
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
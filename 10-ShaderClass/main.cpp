#include <iostream>

#include "GLconfig/core.h"

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"
// 3-Application中的Application.h
#include "Application.h"
#include "GLconfig/shader.h"

// 将VAO和装载shader的程序提升到全局变量
GLuint VAO;
// 全局的Shader对象
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
        "assets/shader/default/vertex.glsl",
        "assets/shader/default/fragment.glsl"
        );
}

// 准备EBO
void prepareEBOBuffer() {
    float positions[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f
    };
    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f
    };
    // 顶点索引顺序数据, 方便复用顶点
    unsigned int indices[] = {
        0, 1, 2,
        2, 1, 3
    };

    GLuint position, color, EBO;
    // VBO
    GL_CALL(glGenBuffers(1, &position));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, position));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

    GL_CALL(glGenBuffers(1, &color));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, color));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

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

    // 绑定EBO, 📌📌这一行不可以省略
    // 📌📌因为glVertexAttribPointer内会自动查询并绑定当前的VBO, 但不会查询EBO
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    // 解绑VAO
    GL_CALL(glBindVertexArray(0));
}

// 执行渲染操作
void render() {
    // 画布清理操作也算渲染操作
    // 执行画布清理操作(用glClearColor设置的颜色来清理(填充)画布)
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();
    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // 使用EBO顶点索引绘制. 加载了EBO后indices参数表示EBO内偏移量
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    shader->end();
}

/*
 * 着色器封装为一个类, 实现文件读取, 编译链接, 查错等功能
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
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
        "assets/shader/vertex.glsl",
        "assets/shader/fragment.glsl"
        );
}

// VBO与多属性数据(顶点+颜色)的绑定
// 1. single buffer: 每种属性放在一个单独的VBO中
void prepareSingleBuffer() {
    // 顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下角
         0.5f, -0.5f, 0.0f, // 右下角
         0.0f,  0.5f, 0.0f  // 顶部
    };
    // 每个顶点的颜色数据.
    // 顶点之间的颜色会通过插值来计算, 有渐变效果
    float colors[] = {
        1.0f, 0.0f, 0.0f, // 红
        0.0f, 1.0f, 0.0f, // 绿
        0.0f, 0.0f, 1.0f  // 蓝
    };

    // single buffer: 每种属性放在一个单独的VBO中
    GLuint verticeVBO, colorVBO;
    GL_CALL(glGenBuffers(1, &verticeVBO));
    GL_CALL(glGenBuffers(1, &colorVBO));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, verticeVBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW));

    // 生成VAO并且绑定
    VAO = 0;
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glBindVertexArray(VAO));

    // 分别将位置和颜色属性的描述信息加入VAO数组中
    // 位置属性
    // 这里又用了glBindBuffer来绑定位置VBO, 是因为只有这样绑定后VAO的属性描述才会与此VBO相关
    // 📌📌==>VAO会保存当前(最近)绑定的VBO的状态
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, verticeVBO));
    GL_CALL(glEnableVertexAttribArray(0));
    // 设置具体的属性描述信息
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

    // 颜色属性
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVBO));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

    // 设置完后解绑VAO, 防止后续的 OpenGL 操作意外地修改当前绑定的 VAO
    GL_CALL(glBindVertexArray(0));
}

// 2. interleaved buffer: 每个顶点的属性放在一个VBO中
void prepareInterleavedBuffer() {
    // 顶点+颜色数据
    float verticesWithColor[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下红
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 右下绿
         0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f  // 顶部蓝
    };

    // interleaved buffer: 每个顶点的属性放在一个VBO中
    GLuint VBO;
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithColor), verticesWithColor, GL_STATIC_DRAW));

    // 生成VAO并且绑定
    // 此时VAO中的两项属性描述都绑定到一个VBO
    VAO = 0;
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glBindVertexArray(VAO));

    // 位置信息
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr));
    // 颜色信息
    GL_CALL(glEnableVertexAttribArray(1));
    // 因为是一个VBO里面存了位置+颜色, 所以读取颜色时需要有偏移量
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));

    // 解绑VAO
    GL_CALL(glBindVertexArray(0));
}

// 准备两个三角形的顶点数据
void prepareMultiTriangleBuffer() {
    // 顶点+颜色数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f,
         0.8f,  0.8f, 0.0f,
         0.8f,  0.0f, 0.0f
    };

    // interleaved buffer: 每个顶点的属性放在一个VBO中
    GLuint VBO;
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    // 生成VAO并且绑定
    // 此时VAO中的两项属性描述都绑定到一个VBO
    VAO = 0;
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glBindVertexArray(VAO));

    // 位置信息
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

    // 解绑VAO
    GL_CALL(glBindVertexArray(0));
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
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT))

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();
    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    shader->end();
}

/*
 * 着色器封装为一个类, 实现文件读取, 编译链接, 查错等功能
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "EBO-使用顺序索引绘制")) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置事件回调
    // 窗体尺寸变化
    APP->setOnResizeCallback(framebufferSizeCallback);
    // 键盘输入
    APP->setOnKeyboardCallback(keyCallback);

    // 设置OpenGL视口
    GL_CALL(glViewport(0, 0, 800, 600));
    // 设置擦除画面时的颜色. (擦除画面其实就是以另一种颜色覆盖当前画面)
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    // 编译着色器
    prepareShader();
    // 初始化VBO等资源
    // prepareSingleBuffer();
    // prepareInterleavedBuffer();
    // prepareMultiTriangleBuffer();
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
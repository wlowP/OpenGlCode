#include <iostream>

// glad的头文件引用必须放在glfw之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"
// 3-Application中的Application.h
#include "Application.h"

// 将VAO和装载shader的程序提升到全局变量
GLuint VAO, shaderProgram;

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
    // 顶点着色器和片段着色器的源代码
    const char* vertexShaderSource = "#version 460 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n\0";
    // 1. 创建Shader程序
    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 输入代码
    // length为nullptr表示字符串以'\0'结尾('\0'结尾表示自然结尾, 无需传递长度)
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    // 编译Shader并检查编译结果
    int success = 0;
    char infoLog[1024];
    glCompileShader(vertexShader);
    // 检查编译结果
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glCompileShader(fragmentShader);
    // 检查编译结果
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 2. 链接Shader程序
    // 创建Shader程序对象(全局变量)
    shaderProgram = glCreateProgram();
    // 将编译好的vs和fs附加到Shader程序对象上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // 链接Shader程序, 形成一个完整的可执行Shader程序
    glLinkProgram(shaderProgram);
    // 检查链接结果
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 清理
    // 编译链接形成可执行Shader程序后, 着色器对象就不需要了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// 准备VBO
// 创建, 销毁, 绑定
void prepareVBO() {
    // 要渲染的数据
    // 顶点数据
    // 这里用的是NDC坐标系, 也就是[-1, 1]之间的坐标, 并且Z轴都设为0
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下角
         0.5f, -0.5f, 0.0f, // 右下角
         0.0f,  0.5f, 0.0f  // 顶部
    };

    // 创建VBO
    GLuint VBO;
    GL_CALL(glGenBuffers(1, &VBO));
    // 绑定VBO
    // 参数一: 要绑定的OpenGL状态机插槽位置. GL_ARRAY_BUFFER: 目标缓冲区, 用于存储顶点数据
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    // 向当前VBO传输数据. 📌会开辟显存空间
    // 参数一: 目标缓冲区. GL_ARRAY_BUFFER: 目标缓冲区, 用于存储顶点数据
    // 参数二: 数据大小. sizeof(vertices): 传输数据的大小
    // 参数三: 数据指针. vertices: 传输数据的指针
    // 参数四: 传输方式. GL_STATIC_DRAW: 静态绘制, 数据不会改变
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
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
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

    // 颜色属性
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, colorVBO));
    GL_CALL(glEnableVertexAttribArray(1));
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));

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
    // 顶点索引顺序数据, 方便复用顶点
    unsigned int indices[] = {
        0, 1, 2,
        2, 1, 3
    };

    GLuint VBO, EBO;
    // VBO
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW));

    // 创建EBO. 注意target是GL_ELEMENT_ARRAY_BUFFER
    GL_CALL(glGenBuffers(1, &EBO));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // VAO
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glBindVertexArray(VAO));

    // 绑定VBO, EBO, 加入属性描述信息
    // GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glEnableVertexAttribArray(0));
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr));

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
    glUseProgram(shaderProgram);
    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);

    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}

/*
* EBO: 代表一段用于存储顶点绘制顺序索引号的显存区域
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
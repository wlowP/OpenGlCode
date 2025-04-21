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
TextureMipMap* textureReisen = nullptr;
TextureMipMap* textureReisen1 = nullptr;
// 当前的模型变换矩阵. (构造函数传递1.0f会初始化为单位矩阵)
glm::mat4 transform(1.0f);
glm::mat4 transform1(1.0f);
// 视图变换矩阵
glm::mat4 viewMatrix(1.0f);
// 正交投影变换矩阵
glm::mat4 perspectiveMatrix(1.0f);

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

// 准备包含uv坐标的缓冲区
void prepareEBOBuffer() {
    // 因为已经加载了正交投影矩阵, 而正交投影变换中包含着NDC坐标归一化的操作
    // 所以准备的顶点数据可以是真正的世界坐标系了
    // 被框进正交投影盒的内容才会显示
    float positions[] = {
        -1.0f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         0.0f, 1.0f, 0.0f,
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
    textureReisen = new TextureMipMap("assets/texture/reisen.jpg", 0);
    textureReisen1 = new TextureMipMap("assets/texture/reisen1.jpg", 0);
}

// 摄像机状态
void prepareCamera() {
    /*
     * 使用glm::lookAt函数来创建视图矩阵
     *  - eye: 相机位置. (📌📌以相机坐标系为原点, xyz轴超出[-1, 1]范围的内容将被裁剪, 不可见)
     *  - center: 相机朝向(看向的位置)
     *  - up: 穹顶方向
     */
    viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void preparePerspectiveProjection() {
    /*
     * 使用glm::perspective函数来创建透视投影矩阵
     *  - fovy: 视场角(弧度制)
     *  - aspect: 近平面的宽高比, 可以与窗体宽高比相同
     *  - near/far: 近/远平面距离, 为正数
     */
    perspectiveMatrix = glm::perspective(
        glm::radians(60.0f),
        (float)APP->getWidth() / (float)APP->getHeight(),
        0.1f,
        100.0f
    );
}

void prepareState() {
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 设置深度测试函数. GL_LESS->保留深度值较小的. 近处遮挡远处
    glDepthFunc(GL_LESS);
    // 设置清除时的深度值. 默认值也为1.0f(远平面)
    glClearDepth(1.0f);
}

// 执行渲染操作
void render() {
    // 画布清理操作也算渲染操作
    // 执行画布清理操作(用glClearColor设置的颜色来清理(填充)画布)
    // 同时清理深度缓存. 默认清理为1.0f(远平面)
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();

    // 通过uniform将采样器绑定到0号纹理单元上
    // -> 让采样器知道要采样哪个纹理单元
    shader->setInt("sampler", 0);
    shader->setMat4("transform", transform);
    shader->setMat4("viewMatrix", viewMatrix);
    shader->setMat4("projectionMatrix", perspectiveMatrix);

    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    textureReisen->bindTexture();
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    // 绘制并移动第二个带纹理的三角形, 以便深度测试
    textureReisen1->bindTexture();
    transform1 = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, -0.5f));
    shader->setMat4("transform", transform1);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    Shader::end();
}

/**
 * OpenGL深度测试
 *  - glEnable(GL_DEPTH_TEST): 启用深度测试
 *  - glDepthFunc(GL_LESS): 设置深度测试函数
 *  - glClear(GL_DEPTH_BUFFER_BIT): 清除深度缓存(与清除画布颜色同时进行)
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "深度测试")) {
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
    // 设置摄像机参数
    prepareCamera();
    // 设置透视投影参数
    preparePerspectiveProjection();
    // 设置OpenGL状态
    prepareState();

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
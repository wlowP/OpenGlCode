#include <iostream>

#include "core.h"

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"

#include "application/Application.h"
#include "application/camera/perspective.h"
#include "application/camera/orthographic.h"
#include "application/camera/trackballCameraController.h"
#include "shader.h"
#include "TextureMipMap.h"

GLuint VAO;
// 封装的着色器程序对象
Shader* shader = nullptr;
// 纹理对象
TextureMipMap* texture = nullptr;
// 当前的模型变换矩阵. (构造函数传递1.0f会初始化为单位矩阵)
glm::mat4 transform(1.0f);

// 相机及其控制器对象
PerspectiveCamera* perspectiveCamera = nullptr;
OrthographicCamera* orthographicCamera = nullptr;
Camera * currentCamera = nullptr; // 当前使用的相机
TrackballCameraController* cameraController = nullptr;

// 窗口尺寸变化的回调
void framebufferSizeCallback(const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
    // 使用glad中的glViewport来动态更新视口的大小
    glViewport(0, 0, width, height);
}

// 键盘输入的回调
void keyCallback(const int key, const int action, int mods) {
    cameraController->onKeyboard(key, action, mods);
}

// 鼠标点击的回调
void mouseCallback(const int button, const int action, const int mods) {
    double x, y;
    APP->getMousePosition(x, y);
    cameraController->onMouse(button, action, x, y);
}

// 鼠标移动的回调
void mouseMoveCallback(const double x, const double y) {
    cameraController->onMouseMove(x, y);
}

// 鼠标滚轮的回调
void mouseScrollCallback(const double offsetX, const double offsetY) {
    cameraController->onMouseScroll(offsetX, offsetY);
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
    texture = new TextureMipMap("assets/texture/reisen.jpg", 0);
    texture->bindTexture();
}

// 摄像机状态
void prepareCamera() {
    perspectiveCamera = new PerspectiveCamera(
        60.0f,
        (float)APP->getWidth() / (float)APP->getHeight(),
        0.1f, 1000.0f
    );
    float orthoBoxSize = 8.0f;
    orthographicCamera = new OrthographicCamera(
        -orthoBoxSize, orthoBoxSize,
        -orthoBoxSize, orthoBoxSize,
        orthoBoxSize, -orthoBoxSize
    );
    // 设置当前的相机
    currentCamera = orthographicCamera;
    cameraController = new TrackballCameraController();
    cameraController->setCamera(currentCamera);
    cameraController->setSensitivity(0.05f);
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
    shader->setMat4("viewMatrix", currentCamera->getViewMatrix());
    shader->setMat4("projectionMatrix", currentCamera->getProjectionMatrix());

    // 📌📌绑定当前的VAO(包含几何结构)
    glBindVertexArray(VAO);

    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // 使用EBO顶点索引绘制. 加载了EBO后indices参数表示EBO内偏移量
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

    Shader::end();
}

/**
 * 相机相关的矩阵变换等操作封装为相机系统类
 *  1. 数据
 *      - 视图变换矩阵的参数及计算
 *      - 投影矩阵包含正交投影和透视投影两种, 于是设计为两个子类
 *  2. 控制
 *      - 根据键盘鼠标的输入, 控制相机的移动和旋转
 *      - 设置回调函数的API在Application类中
 *      - 分为游戏相机以及轨迹球相机两种, 于是也设计为两个子类
 *      - CameraController的update方法要在每一帧调用
 *      -- 轨迹球相机
 *          - 鼠标左键拖动可以旋转物体, 右键拖动可以平移物体, 滚轮缩放物体
 *          - 注意正交缩放和透视缩放的区别. 正交缩放的倍率是非线性的(指数函数), 而透视缩放是线性的(直接平移相机, 近大远小)
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "封装为相机系统类")) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置事件回调
    // 窗体尺寸变化
    APP->setOnResizeCallback(framebufferSizeCallback);
    // 键盘输入
    APP->setOnKeyboardCallback(keyCallback);
    // 鼠标点击
    APP->setOnMouseCallback(mouseCallback);
    // 鼠标移动
    APP->setOnMouseMoveCallback(mouseMoveCallback);
    // 鼠标滚轮
    APP->setOnMouseScrollCallback(mouseScrollCallback);

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

    // 3. 执行窗体循环. 📌📌每次循环为一帧
    // 窗体只要保持打开, 就会一直循环
    while (APP->update()) {
        cameraController->update();
        // 渲染操作
        render();
    }

    // 4. 清理和关闭
    APP->destroy();

    return 0;
}
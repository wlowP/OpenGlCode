#include <iostream>

#include "GLconfig/core.h"

#include "application/Application.h"
#include "application/camera/perspectiveCamera.h"
#include "application/camera/orthographicCamera.h"
#include "application/camera/trackballCameraController.h"
#include "application/camera/gameCameraController.h"
#include "GLconfig/geometry.h"
#include "GLconfig/shader.h"
#include "GLconfig/Texture.h"

// 渲染的几何体对象
Geometry* geometry = nullptr;
// 封装的着色器程序对象
Shader* shader = nullptr;
// 纹理对象
TextureMipMap* texture = nullptr;

// 相机及其控制器对象
PerspectiveCamera* perspectiveCamera = nullptr;
OrthographicCamera* orthographicCamera = nullptr;
Camera * currentCamera = nullptr; // 当前使用的相机
TrackballCameraController* trackballCameraController = nullptr;
GameCameraController* gameCameraController = nullptr;
CameraController* currentCameraController = nullptr; // 当前使用的相机控制器

// 窗口尺寸变化的回调
void framebufferSizeCallback(const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
    // 使用glad中的glViewport来动态更新视口的大小
    glViewport(0, 0, width, height);
}

// 键盘输入的回调
void keyCallback(const int key, const int action, int mods) {
    // 如果按下ESC键, 则退出程序
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        APP->closeWindow();
        return;
    }
    currentCameraController->onKeyboard(key, action, mods);
}

// 鼠标点击的回调
void mouseCallback(const int button, const int action, const int mods) {
    double x, y;
    APP->getMousePosition(x, y);
    currentCameraController->onMouse(button, action, x, y);
}

// 鼠标移动的回调
void mouseMoveCallback(const double x, const double y) {
    currentCameraController->onMouseMove(x, y);
}

// 鼠标滚轮的回调
void mouseScrollCallback(const double offsetX, const double offsetY) {
    currentCameraController->onMouseScroll(offsetX, offsetY);
}

// 定义和编译着色器
void prepareShader() {
    shader = new Shader(
        "assets/shader/default/vertex.glsl",
        "assets/shader/default/fragment.glsl"
    );
}

// 创建几何体, 获取对应的VAO
void prepareVAO() {
    // geometry = Geometry::createBox(6.0f, 6.0f, 6.0f);
    geometry = Geometry::createSphere(6.0f, 30, 30);
}

// 纹理加载
void prepareTexture() {
    texture = new TextureMipMap("assets/texture/reisen.jpg", 0);
    texture->bindTexture();
}

// 摄像机状态
void prepareCamera() {
    // ===相机对象===
    perspectiveCamera = new PerspectiveCamera(
        60.0f,
        (float)APP->getWidth() / (float)APP->getHeight(),
        0.1f, 1000.0f
    );
    float orthoBoxSize = 18.0f;
    orthographicCamera = new OrthographicCamera(
        -orthoBoxSize, orthoBoxSize,
        -orthoBoxSize, orthoBoxSize,
        orthoBoxSize, -orthoBoxSize
    );
    // 设置当前的相机
    currentCamera = perspectiveCamera;

    // ===相机控制器对象===
    trackballCameraController = new TrackballCameraController();
    gameCameraController = new GameCameraController();

    // 设置当前的相机控制器
    currentCameraController = gameCameraController;
    // 游戏控制模式下隐藏并捕获鼠标光标
    if (currentCameraController == gameCameraController) {
        APP->setCursorVisible(false);
    }
    currentCameraController->setCamera(currentCamera);
}

// 设置OpenGL状态机参数
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentCameraController->update();

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();

    // 通过uniform将采样器绑定到0号纹理单元上
    // -> 让采样器知道要采样哪个纹理单元
    shader->setInt("sampler", 0);
    shader->setMat4("viewMatrix", currentCamera->getViewMatrix());
    shader->setMat4("projectionMatrix", currentCamera->getProjectionMatrix());

    shader->setMat4("transform", Geometry::getModelMatrix());
    glBindVertexArray(geometry->getVAO());
    // 📌📌绑定当前的VAO(包含几何结构)
    // glDrawArrays(GL_TRIANGLES, 0, 6);
    // 使用EBO顶点索引绘制. 加载了EBO后indices参数表示EBO内偏移量
    glDrawElements(GL_TRIANGLES, geometry->getIndicesCount(), GL_UNSIGNED_INT, nullptr);

    Shader::end();
}

/**
 * 将创建几何体(VBO, VAO, EBO的创建和绑定)的过程封装为几何体类
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "VBO, VAO, EBO等封装为几何体类")) {
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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // 编译着色器
    prepareShader();
    // 初始化VBO, VAO等资源
    prepareVAO();
    // 加载纹理
    prepareTexture();
    // 设置摄像机参数
    prepareCamera();
    // 设置OpenGL状态机参数
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
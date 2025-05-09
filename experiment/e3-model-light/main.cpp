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
#include "application/model.h"

// 渲染的几何体对象
GeometryInstance* geometry = nullptr;
// 光源对象
GeometryInstance* lightSource = nullptr;
// 模型对象
Model* model = nullptr;
// 封装的着色器程序对象
Shader* shader = nullptr;
Shader* lightSourceShader = nullptr;
// 纹理对象
Texture* texture = nullptr;

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
    if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
        model->smooth();
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
        "assets/shader/lightMap/vertex.glsl",
        "assets/shader/lightMap/fragment.glsl"
    );
    lightSourceShader = new Shader(
        "assets/shader/lightSource/vertex.glsl",
        "assets/shader/lightSource/fragment.glsl"
    );
}

// 创建几何体, 获取对应的VAO
void prepareGeometries() {
    // 场景几何体
    Geometry* box = Geometry::createBox(1, 1, 1, glm::vec3(1.0, 0.5, 0.31));
    box->loadTexture("assets/texture/reisen.jpg");
    geometry = new GeometryInstance(box, -4, 0, 0);
    // geometry->scale(3, 3, 3);
    geometry->updateMatrix =
        glm::translate(geometry->updateMatrix, geometry->getWorldCenter() * 1.0f) *
        glm::rotate(geometry->updateMatrix, glm::radians(-0.1f), glm::vec3(0, 1, 0)) *
        glm::translate(geometry->updateMatrix, geometry->getWorldCenter() * -1.0f);
    geometry->useTexture = false;

    // 光源几何体
    Geometry* lightBox = Geometry::createSphere(1, 60, 60, glm::vec3(1, 1, 1));
    lightSource = new GeometryInstance(lightBox, 1.2, 1, 2);
    lightSource->scale(0.1, 0.1, 0.1);
    lightSource->updateMatrix = glm::rotate(lightSource->updateMatrix, glm::radians(0.2f), glm::vec3(0, 1, 0));
    lightSource->useTexture = false;

    // 加载的几何模型
    model = new Model("D:/code/repositories/OpenGlCode/experiment/e3-model-light/assets/model/dog/dog.obj", 1, 0.3f, -0.53f * 0.3f);
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
    currentCamera = orthographicCamera;

    // ===相机控制器对象===
    trackballCameraController = new TrackballCameraController();
    gameCameraController = new GameCameraController();

    // 设置当前的相机控制器
    currentCameraController = trackballCameraController;
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

    // ==================绘制光源物体. 采用另一个着色器程序, 防止光源本身被影响==================
    lightSourceShader->begin();
    lightSourceShader->setInt("sampler", 0);
    lightSourceShader->setMat4("viewMatrix", currentCamera->getViewMatrix());
    lightSourceShader->setMat4("projectionMatrix", currentCamera->getProjectionMatrix());

    const Geometry* lightSourceModel = lightSource->geometry;
    lightSourceModel->bind();
    lightSourceShader->setBool("useTexture", lightSource->useTexture);
    lightSource->update();
    lightSourceShader->setMat4("model", lightSource->getModelMatrix());
    glDrawElements(lightSourceModel->getPrimitiveType(), lightSourceModel->getIndicesCount(), GL_UNSIGNED_INT, nullptr);

    // ==================绘制几何体场景==================

    // 📌📌绑定当前的shaderProgram(选定一个材质)
    // glUseProgram(shaderProgram);
    shader->begin();
    // 通过uniform将采样器绑定到0号纹理单元上
    // -> 让采样器知道要采样哪个纹理单元
    shader->setInt("sampler", 0);
    // 同时也将纹理设置给物体光照材质的采样器(光照贴图. 包含漫反射贴图和镜面反射贴图)
    shader->setInt("material.diffuse", 0);
    shader->setInt("material.specular", 0);
    shader->setMat4("viewMatrix", currentCamera->getViewMatrix());
    shader->setMat4("projectionMatrix", currentCamera->getProjectionMatrix());

    // 光源属性
    shader->setVec3("lightPosition", lightSource->getWorldCenter());
    shader->setVec3("viewPosition", currentCamera->position);
    shader->setVec3("lightSource.ambient", 0.2f, 0.2f, 0.2f);
    shader->setVec3("lightSource.diffuse", 0.5f, 0.5f, 0.5f);
    shader->setVec3("lightSource.specular", 0.8f, 0.8f, 0.8f);

    const Geometry* geometryModel = geometry->geometry;
    // 绑定几何体对应的模型的VAO
    geometryModel->bind();
    shader->setBool("useTexture", geometry->useTexture);
    geometry->update();
    // 设置变换矩阵以及法线矩阵
    shader->setMat4("model", geometry->getModelMatrix());
    shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(geometry->getModelMatrix()))));
    // 物体材质属性
    shader->setVec3("material.ambient", geometryModel->material.ambient);
    shader->setVec3("material.diffuse", geometryModel->material.diffuse);
    shader->setVec3("material.specular", geometryModel->material.specular);
    shader->setFloat("material.shininess", geometryModel->material.shininess);
    glDrawElements(geometryModel->getPrimitiveType(), geometryModel->getIndicesCount(), GL_UNSIGNED_INT, nullptr);

    // ======绘制模型
    auto transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f)); // 把模型移到世界原点
    transform = glm::scale(transform, glm::vec3(0.015, 0.015, 0.015));	// 有些模型太大了缩小一点
    // 记得传递法线矩阵, 否则光照会异常(比如明明光源在背后但是前面却反光)
    shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));
    shader->setMat4("model", transform);
    model->draw(shader);

    Shader::end();
}

/**
 * 将创建几何体(VBO, VAO, EBO的创建和绑定)的过程封装为几何体类
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "光照与模型加载")) {
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 编译着色器
    prepareShader();
    // 初始化VBO, VAO等资源
    prepareGeometries();
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
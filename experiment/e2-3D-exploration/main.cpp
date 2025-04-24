#include <iostream>
#include <thread>

#include "core.h"

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"

#include "application/Application.h"
#include "application/camera/perspectiveCamera.h"
#include "application/camera/gameCameraController.h"
#include "application/camera/gameControlMoveStrategy.h"
#include "GLconfig/geometry.h"
#include "shader.h"
#include "application/util.h"

#include "application/Global.h"

// 封装的着色器程序对象
Shader* shader = nullptr;
// 相机及其控制器对象
PerspectiveCamera* perspectiveCamera = nullptr;
Camera * currentCamera = nullptr; // 当前使用的相机
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

// 创建几何体, 组成地图场景
void prepareGeometries() {
    // 几何体模型
    Geometry* reisenBlock = Geometry::createBox(1.0f, 1.0f, 1.0f);
    reisenBlock->loadTexture("assets/texture/reisen.jpg");
    Geometry* brickBlock = Geometry::createBox(1.0f, 1.0f, 1.0f);
    brickBlock->loadTexture("assets/texture/bricks.png");
    Geometry* goldBlock = Geometry::createBox(1.0f, 1.0f, 1.0f);
    goldBlock->loadTexture("assets/texture/gold_block.png");
    Geometry* floorPlane = Geometry::createPlane(80.0f, 80.0f, 40);
    floorPlane->loadTexture("assets/texture/wall.jpg");

    // 几何体实例
    auto* reisenBlockInstance = new GeometryInstance(reisenBlock, 0, 1, -50);
    reisenBlockInstance->scale(glm::vec3(2, 2, 2));
    reisenBlockInstance->updateMatrix = glm::rotate(reisenBlockInstance->updateMatrix, glm::radians(0.1f), glm::vec3(0, 1, 0));
    geometries.push_back(reisenBlockInstance);

    // 生成迷宫
    // 终点处的金块
    auto* goldBlockInstance = new GeometryInstance(goldBlock, goalPos);
    goldBlockInstance->scale(0.5f, 0.5f, 0.5f);
    goldBlockInstance->detectCollision = false;
    // 绕自身旋转, 先移到原点, 旋转完再移回来
    goldBlockInstance->updateMatrix =
        glm::translate(goldBlockInstance->updateMatrix, goldBlockInstance->getWorldCenter() * 1.0f) *
        glm::rotate(goldBlockInstance->updateMatrix, glm::radians(0.5f), glm::vec3(0, 1, 0)) *
        glm::translate(goldBlockInstance->updateMatrix, goldBlockInstance->getWorldCenter() * -1.0f);
    geometries.push_back(goldBlockInstance);
    // 迷宫矩阵(1->墙体, 0->空气)
    const vector<vector<int>> maze = generateMaze(mazeRows, mazeCols, 1, 1, mazeCols - 2, mazeRows - 2);
    std::cout << "generating maze..." << std::endl;
    // 根据maze数组填充new GeometryInstance(brickBlock, x, y, z)
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[0].size(); j++) {
            if (maze[i][j] == 1) {
                auto* brickBlockInstance = new GeometryInstance(brickBlock, j, 0.5f, -i);
                // brickBlockInstance->scale(glm::vec3(2, 2, 2));
                geometries.push_back(brickBlockInstance);
            }
            cout << maze[i][j] << " ";
        }
        cout << std::endl;
    }

    auto* floorPlaneInstance = new GeometryInstance(floorPlane, 0, 0, 0);
    geometries.push_back(floorPlaneInstance);
}

// 摄像机状态
void prepareCamera() {
    // ===相机对象===
    perspectiveCamera = new PerspectiveCamera(
        60.0f,
        (float)APP->getWidth() / (float)APP->getHeight(),
        0.1f, 1000.0f
    );
    // 设置相机初始位置
    perspectiveCamera->position = glm::vec3(1.0f, 0.2f, -1.0f);
    // 设置当前的相机
    currentCamera = perspectiveCamera;

    // ===相机控制器对象===
    gameCameraController = new GameCameraController(new OrthoMove());
    // 初始化游戏控制方式下的相机碰撞体积
    gameCameraController->setBoundingSpace(currentCamera, 0.2f);
    // 设置当前的相机控制器
    currentCameraController = gameCameraController;
    // 游戏控制模式下隐藏并捕获鼠标光标
    APP->setCursorVisible(false);
    currentCameraController->setCamera(currentCamera);
}

// 设置OpenGL状态机参数
void prepareState() {
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // 设置深度测试函数. GL_LESS->保留深度值较小的. 近处遮挡远处
    glClearDepth(1.0f); // 设置清除时的深度值. 默认值也为1.0f(远平面)
}

// 命令行线程
void command() {
    std::string cmd;
    while (true) {
        std::cin >> cmd;
        if (cmd == "/clear") {
            geometries.clear();
            std::cout << "cleared all geometries" << std::endl;
        } else if (cmd == "/center") {
            std::cout << "geometries center: " << std::endl;
            for (const auto geometry : geometries) {
                std::cout << glm::to_string(geometry->getWorldCenter()) << std::endl;
            }
        } else if (cmd == "/exit") {
            APP->closeWindow();
            std::cout << "shutting down..." << std::endl;
            break;
        } else {
            std::cout << "unknown command: " << cmd << std::endl;
        }
    }
}

// 执行渲染操作
void render() {
    // 执行画布清理操作(用glClearColor设置的颜色来清理(填充)画布)
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // 相机在每一帧都需要更新的操作. 比如游戏相机的WSAD移动
    currentCameraController->update();

    shader->begin();

    shader->setInt("sampler", 0);
    shader->setMat4("viewMatrix", currentCamera->getViewMatrix());
    shader->setMat4("projectionMatrix", currentCamera->getProjectionMatrix());

    // 循环渲染所有的几何体
    for (auto instance : geometries) {
        Geometry* geometry = instance->geometry;
        // 绑定几何体的VAO
        geometry->bind();
        instance->update();
        // 设置变换矩阵
        shader->setMat4("transform", instance->getModelMatrix());
        // 绘制几何体
        glDrawElements(geometry->getPrimitiveType(), geometry->getIndicesCount(), GL_UNSIGNED_INT, nullptr);
    }

    Shader::end();
}

/**
 * 将创建几何体(VBO, VAO, EBO的创建和绑定)的过程封装为几何体类
 */
int main() {
    APP->test();
    if (!APP->init(1500, 1125, "3D 场景漫游互动")) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置事件回调
    APP->setOnResizeCallback(framebufferSizeCallback);// 窗体尺寸变化
    APP->setOnKeyboardCallback(keyCallback);// 键盘输入
    APP->setOnMouseCallback(mouseCallback);// 鼠标点击
    APP->setOnMouseMoveCallback(mouseMoveCallback);// 鼠标移动
    APP->setOnMouseScrollCallback(mouseScrollCallback);// 鼠标滚轮

    // 设置擦除画面时的颜色. (擦除画面其实就是以另一种颜色覆盖当前画面)
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    // 编译着色器
    prepareShader();
    // 初始化几何体列表
    prepareGeometries();
    // 设置摄像机参数
    prepareCamera();
    // 设置OpenGL状态机参数
    prepareState();
    std::thread t(command);
    t.detach();

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
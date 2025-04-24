//
// Created by ROG on 2025/4/21.
//

#include "gameCameraController.h"
#include "../Global.h"
#include "../Application.h"

GameCameraController::GameCameraController(GameControlMoveStrategy* moveStrategy) : moveStrategy(moveStrategy) {
}

GameCameraController::GameCameraController() {
    // 默认移动方式是允许任意方向的移动
    moveStrategy = new FreeMove();
}
GameCameraController::~GameCameraController() = default;

void GameCameraController::setBoundingSpace(const Camera* camera, float boundingRadius) {
    // 初始化碰撞体积
    boundingSphere.center = camera->position;
    boundingSphere.radius = boundingRadius;

    boundingBox.min = camera->position - glm::vec3(boundingRadius, boundingRadius, boundingRadius);
    boundingBox.max = camera->position + glm::vec3(boundingRadius, boundingRadius, boundingRadius);
}


void GameCameraController::onKeyboard(int key, int action, int mods) {
    // 先调用父类的onKeyboard函数, 记录按键状态
    CameraController::onKeyboard(key, action, mods);

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        // 切换光标隐藏状态
        APP->setCursorVisible(!APP->isCursorVisible());
    }
}


void GameCameraController::onMouseMove(double x, double y) {
    float dx = (x - mouseX) * sensitivity;
    float dy = (y - mouseY) * sensitivity;

    pitch(-dy);
    yaw(-dx);

    mouseX = x;
    mouseY = y;
}

void GameCameraController::pitch(float angle) {
    // 限制up的旋转角度不能超过90度
    pitchAngle += angle;
    if (pitchAngle > 89.0f || pitchAngle < -89.0f) {
        pitchAngle = glm::clamp(pitchAngle, -89.0f, 89.0f);
        return;
    }

    // 游戏控制器的俯仰角变换只需改变up, 不改变position
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), camera->right);
    camera->up = rotate * glm::vec4(camera->up, 0.0f);
}

void GameCameraController::yaw(float angle) {
    // 注意游戏控制的yaw旋转要绕着世界的y轴进行
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->right = rotate * glm::vec4(camera->right, 0.0f);
    camera->up = rotate * glm::vec4(camera->up, 0.0f);
}

void GameCameraController::update() {
    // 最终移动方向, 注意别忘了归一化
    glm::vec3 direction;

    // glm::vec3 front = glm::cross(camera->up, camera->right);
    /*if (keyState[GLFW_KEY_W]) {
        direction += front;
    }
    if (keyState[GLFW_KEY_S]) {
        direction -= front;
    }
    if (keyState[GLFW_KEY_A]) {
        direction -= camera->right;
    }
    if (keyState[GLFW_KEY_D]) {
        direction += camera->right;
    }
    if (keyState[GLFW_KEY_SPACE]) {
        direction += camera->up;
    }
    if (keyState[GLFW_KEY_LEFT_SHIFT]) {
        direction -= camera->up;
    }*/
    // 简化写法
    direction = moveStrategy->computeDirection(camera->up, camera->right,
                                              (float)(keyState[GLFW_KEY_W] - keyState[GLFW_KEY_S]),
                                              (float)(keyState[GLFW_KEY_D] - keyState[GLFW_KEY_A]),
                                              (float)(keyState[GLFW_KEY_SPACE] - keyState[GLFW_KEY_LEFT_SHIFT]));
    // 注意direction长度可能为0
    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
        glm::vec3 stride = direction * moveSpeed;

        // 检测碰撞
        for (const auto g : geometries) {
            if (checkCollision(g, stride)) {
                std::cout << "collision detected" << std::endl;
                return;
            }
        }

        camera->position += stride;
        // 更新相机的包围球和包围盒
        boundingSphere.center = camera->position;
        boundingBox.min += stride;
        boundingBox.max += stride;
    }
    // 不移动的时候也可以检测被动的碰撞
}

bool GameCameraController::checkCollision(GeometryInstance* b, glm::vec3& stride) {
    // 阶段1：包围球快速排除
    auto sphereA = getBoundingSphere();
    sphereA.center += stride;
    auto sphereB = b->getBoundingSphere();
    if (!isCollide(sphereA, sphereB)) {
        return false;
    }

    // 阶段2：AABB检测
    auto boxA = getBoundingBox(); // 根据顶点变换计算实际AABB
    boxA.min += stride;
    boxA.max += stride;
    auto boxB = b->getBoundingBox();
    if (!isCollide(boxA, boxB)) {
        return false;
    }

    return true;
}

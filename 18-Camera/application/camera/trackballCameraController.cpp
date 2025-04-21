//
// Created by ROG on 2025/4/21.
//

#include "trackballCameraController.h"

TrackballCameraController::TrackballCameraController() = default;
TrackballCameraController::~TrackballCameraController() = default;

void TrackballCameraController::onMouseMove(double x, double y) {
    if (mouseLeftDown) {
        // 左键按住拖动可以旋转物体, 实则为相机绕着一个球心旋转
        // 鼠标左键按下, 进行轨迹球旋转, 调整相机的各种参数
        // 计算鼠标移动的距离, 在乘以敏感度系数
        float dx = (x - mouseX) * sensitivity;
        float dy = (y - mouseY) * sensitivity;

        // 分别进行俯仰角和偏航角的变换. dx和dy当做弧度传递给glm::rotate. 加负号更符合人操控直觉
        pitch(-dy);
        yaw(-dx);
    } else if (mouseRightDown) {
        // 右键按住拖动可以移动物体, 实则为移动相机
        float dx = (x - mouseX) * moveSpeed;
        float dy = (y - mouseY) * moveSpeed;

        translate(-dx, dy);
    }

    // 最后别忘了更新鼠标位置
    mouseX = x;
    mouseY = y;
}

// 是一个增量式的变换. 因为每次调用glm::rotate时, camera->right已经是经过上一轮变换的了
void TrackballCameraController::pitch(float angle) {
    // 俯仰角变换, 包含了相机的up和position两个参数的变换, 可看做绕相机本身的"x轴"(right方向)旋转
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, camera->right);
    // 注意齐次坐标系中w=0表示方向. 📌📌glm库底层处理后, 4维向量赋值给3维向量时会自动丢弃w分量
    camera->up = rotate * glm::vec4(camera->up, 0.0f);
    // 位置也跟着一起旋转. 注意rotate的旋转轴是从原点出发
    camera->position = rotate * glm::vec4(camera->position, 1.0f);
}

void TrackballCameraController::yaw(float angle) {
    // 偏航角变换. 相机的up, right, position都会变化, 可看做绕世界y轴的旋转
    glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
    camera->up = rotate * glm::vec4(camera->up, 0.0f);
    camera->right = rotate * glm::vec4(camera->right, 0.0f);
    camera->position = rotate * glm::vec4(camera->position, 1.0f);
}

void TrackballCameraController::translate(float x, float y) {
    // 注意要沿着相机本身的right/up方向移动, 而不是直接加上(x, y, 0), 这样是沿着绝对世界坐标系移动
    camera->position += camera->right * x;
    camera->position += camera->up * y;
}

void TrackballCameraController::onMouseScroll(double offsetX, double offsetY) {
    camera->zoom(offsetY * zoomSpeed); // offsetY∈{-1, 1}
}


//
// Created by ROG on 2025/4/20.
//

#include "camera.h"

Camera::Camera() = default;
Camera::Camera(const glm::vec3& position, const glm::vec3& up, const glm::vec3& right)
    : position(position), up(up), right(right) {}

Camera::~Camera() = default;

glm::mat4 Camera::getViewMatrix() const {
    // 计算视图矩阵
    return glm::lookAt(position,
       // 目标点, 注意是一个点的坐标, 与position本身相减才得出视线方向
       // 相机的front方向用up和right叉乘得到
       position + glm::cross(up, right),
       up
    );
}

glm::mat4 Camera::getProjectionMatrix() const {
    // 父类默认返回单位矩阵, 具体实现由子类实现
    return glm::identity<glm::mat4>();
}

void Camera::zoom(float deltaScale) {

}
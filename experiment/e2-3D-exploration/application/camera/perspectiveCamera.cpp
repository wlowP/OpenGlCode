//
// Created by ROG on 2025/4/20.
//

#include "perspectiveCamera.h"

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
    : fovy(fovy), aspect(aspect), near(near), far(far) {
}

PerspectiveCamera::~PerspectiveCamera() = default;

glm::mat4 PerspectiveCamera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fovy), aspect, near, far);
}

void PerspectiveCamera::zoom(float deltaScale) {
    // 透视缩放, 因为近大远小, 所以可以直接将相机沿着视线方向移动
    // 叉乘up和right得到视线方向, 然后归一化
    glm::vec3 front = glm::normalize(glm::cross(up, right));

    position += front * deltaScale;
}

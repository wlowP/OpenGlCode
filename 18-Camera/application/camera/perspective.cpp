//
// Created by ROG on 2025/4/20.
//

#include "perspective.h"

PerspectiveCamera::PerspectiveCamera(float fovy, float aspect, float near, float far)
    : fovy(fovy), aspect(aspect), near(near), far(far) {
}

PerspectiveCamera::~PerspectiveCamera() = default;

glm::mat4 PerspectiveCamera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fovy), aspect, near, far);
}
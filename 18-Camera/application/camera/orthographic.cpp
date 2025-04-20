//
// Created by ROG on 2025/4/20.
//

#include "orthographic.h"

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far)
    : left(left), right(right), bottom(bottom), top(top), near(near), far(far) {
}

OrthographicCamera::~OrthographicCamera() = default;

glm::mat4 OrthographicCamera::getProjectionMatrix() const {
    return glm::ortho(left, right, bottom, top, near, far);
}
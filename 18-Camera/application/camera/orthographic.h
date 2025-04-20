//
// Created by ROG on 2025/4/20.
//

#ifndef ORTHOGRAPHIC_H
#define ORTHOGRAPHIC_H

#include "camera.h"

class OrthographicCamera : public Camera {
public:
    OrthographicCamera(float left, float right, float bottom, float top, float near, float far);
    ~OrthographicCamera();

    glm::mat4 getProjectionMatrix() const override;

    // 正交投影的参数
    float left{0.0f};
    float right{0.0f};
    float bottom{0.0f};
    float top{0.0f};
    float near{0.0f};
    float far{0.0f};
};

#endif //ORTHOGRAPHIC_H

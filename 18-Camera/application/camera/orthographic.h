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

    // 正交投影的参数
    float left{0.0f};
    float right{0.0f};
    float bottom{0.0f};
    float top{0.0f};
    float near{0.0f};
    float far{0.0f};

    // 用于缩放的比例因子. 每一帧做出的缩放变化量会累加到此
    // 📌最终图像会缩放为原来的2^scale倍
    float scale{0.0f};

    glm::mat4 getProjectionMatrix() const override;

    void zoom(float deltaScale) override;
};

#endif //ORTHOGRAPHIC_H

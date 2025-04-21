//
// Created by ROG on 2025/4/20.
//

#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fovy, float aspect, float near, float far);
    ~PerspectiveCamera();

    // 透视投影的参数
    // fovy单位是角度, near和far都应当是正数
    float fovy{0.0f};
    float aspect{0.0f};
    float near{0.0f};
    float far{0.0f};

    glm::mat4 getProjectionMatrix() const override;

    void zoom(float deltaScale) override;
};

#endif //PERSPECTIVE_H

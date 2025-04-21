//
// Created by ROG on 2025/4/21.
//

#ifndef TRACKBALLCAMERACONTROLLER_H
#define TRACKBALLCAMERACONTROLLER_H

#include "cameraController.h"

class TrackballCameraController : public CameraController {
public:
    TrackballCameraController();
    ~TrackballCameraController();

    void onMouseMove(double x, double y) override;

private:
    // 俯仰角变换(上下点头)
    void pitch(float angle);
    // 偏航角变换(左右摇头)
    void yaw(float angle);
    // 平移变换(挪动相机)
    void translate(float x, float y);
};

#endif //TRACKBALLCAMERACONTROLLER_H

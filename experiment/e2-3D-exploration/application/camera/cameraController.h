//
// Created by ROG on 2025/4/20.
//

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "core.h"
#include "camera.h"
#include <map>

class CameraController {
public:
    CameraController();
    virtual ~CameraController();

    virtual void onMouse(int button, int action, double x, double y);
    virtual void onMouseMove(double x, double y);
    virtual void onKeyboard(int key, int action, int mods);
    virtual void onMouseScroll(double offsetX, double offsetY);// offsetX, offsetY∈{-1, 1}

    // 需要每一帧更新的行为
    virtual void update();

    // 各种setter
    void setCamera(Camera* camera) {this->camera = camera;}
    void setSensitivity(float sensitivity) {this->sensitivity = sensitivity;}
    void setTranslationpeed(float translationSpeed) {this->translationSpeed = translationSpeed;}
    void setZoomSpeed(float zoomSpeed) {this->zoomSpeed = zoomSpeed;}
protected:
    // 鼠标按键状态
    bool mouseLeftDown = false;
    bool mouseRightDown = false;
    bool mouseMiddleDown = false;
    // 当前鼠标的位置
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    // 灵敏度
    float sensitivity = 0.05f; // 旋转
    float translationSpeed = 0.01f; // 平移
    float zoomSpeed = 0.2f; // 缩放

    // 键盘按键状态. K-V = 按键码-是否按下
    std::map<int, bool> keyState;

    // 当前正在控制的摄像机
    Camera* camera = nullptr;
};

#endif //CAMERACONTROLLER_H

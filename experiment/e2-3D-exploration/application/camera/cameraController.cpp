//
// Created by ROG on 2025/4/20.
//
#include <iostream>

#include "cameraController.h"


CameraController::CameraController() = default;
CameraController::~CameraController() = default;

void CameraController::onMouse(int button, int action, double x, double y) {
    // 如果当前按键按下, 记录鼠标位置
    bool pressed = (action == GLFW_PRESS);
    if (pressed) {
        mouseX = x;
        mouseY = y;
    }
    // 设置鼠标按键状态
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouseLeftDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouseRightDown = pressed;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouseMiddleDown = pressed;
            break;
        default: ;
    }
}

void CameraController::onMouseMove(double x, double y) {
    // 子类没有公共的部分, 留到子类实现...
}

// 仅负责记录按键状态. 具体的处理逻辑在子类的update函数中实现
void CameraController::onKeyboard(int key, int action, int mods) {
    // 此处不处理一直按住的情况, 留到子类的update函数中处理
    if (action == GLFW_REPEAT) {
        return;
    }

    bool pressed = (action == GLFW_PRESS);

    // 设置键盘按键状态在keyMap中
    keyState[key] = pressed;
}

void CameraController::onMouseScroll(double offsetX, double offsetY) {

}


void CameraController::update() {
    // 子类没有公共的部分, 留到子类实现...
}

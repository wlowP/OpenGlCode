//
// Created by ROG on 2025/4/21.
//

#ifndef GAMECAMERACONTROLLER_H
#define GAMECAMERACONTROLLER_H

#include "cameraController.h"
#include "gameControlMoveStrategy.h"

class GameCameraController : public CameraController {
public:
    GameCameraController();
    GameCameraController(GameControlMoveStrategy* moveStrategy) : moveStrategy(moveStrategy) {};
    ~GameCameraController();

    // 鼠标移动直接旋转视角
    void onMouseMove(double x, double y) override;

    // 每一帧持续性的更新, 比如按住WASD一直移动
    void update() override;

    void setMoveSpeed(float moveSpeed) { this->moveSpeed = moveSpeed; }
protected:
    // 记录俯仰角的累计变化量, 以便检测是否超过90度
    float pitchAngle = 0.0f;
    // WASD移动速度
    float moveSpeed = 0.02f;

    // 相机控制器的移动策略, 默认是允许任意方向的移动(构造函数中初始化)
    GameControlMoveStrategy* moveStrategy;

    void pitch(float angle);
    void yaw(float angle);
};

#endif //GAMECAMERACONTROLLER_H

//
// Created by ROG on 2025/4/21.
//

#ifndef GAMECAMERACONTROLLER_H
#define GAMECAMERACONTROLLER_H

#include "cameraController.h"
#include "gameControlMoveStrategy.h"
#include "../../GLconfig/geometry.h"

class GameCameraController : public CameraController {
public:
    GameCameraController();
    GameCameraController(GameControlMoveStrategy* moveStrategy);

    ~GameCameraController();

    // 鼠标移动直接旋转视角
    void onMouseMove(double x, double y) override;

    // 游戏控制需要监听一些特殊按键
    void onKeyboard(int key, int action, int mods) override;

    // 每一帧持续性的更新, 比如按住WASD一直移动
    void update() override;

    void setMoveSpeed(float moveSpeed) { this->moveSpeed = moveSpeed; }
    // 设置碰撞体积
    void setBoundingSpace(const Camera* camera, float boundingRadius);

    BoundingSphere& getBoundingSphere() { return boundingSphere; }
    BoundingBox& getBoundingBox() { return boundingBox; }

    void setMoveStrategy(GameControlMoveStrategy* moveStrategy) { this->moveStrategy = moveStrategy; }
private:
    // 记录俯仰角的累计变化量, 以便检测是否超过90度
    float pitchAngle = 0.0f;
    // WASD移动速度
    float moveSpeed = 0.02f;
    // 碰撞检测: 相机的包围球和AABB包围盒, 默认尺寸是0.1f
    BoundingSphere boundingSphere{ glm::vec3(0.0f), 0.1f };
    BoundingBox boundingBox{ glm::vec3{-0.1f}, glm::vec3{0.1f} };

    // 相机控制器的移动策略, 默认是允许任意方向的移动(构造函数中初始化)
    GameControlMoveStrategy* moveStrategy;

    // 检测相机移动stride距离时是否会与几何体实例相撞
    bool checkCollision(GeometryInstance* b, const glm::vec3& stride);

    void pitch(float angle);
    void yaw(float angle);
};

#endif //GAMECAMERACONTROLLER_H

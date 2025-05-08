//
// Created by ROG on 2025/4/22.
//

#ifndef GAMECONTROLMOVESTRATEGY_H
#define GAMECONTROLMOVESTRATEGY_H

#include "../../GLconfig/core.h"

/**
 *  游戏控制器的移动策略(比如WSAD是否只能在y=当前高度的平面上移动, 是否允许上下移动)
 */
class GameControlMoveStrategy {
public:
    virtual ~GameControlMoveStrategy() = default;
    /**
     * 计算游戏控制相机的移动方向, 模长可能为0(比如同时按下W和S)
     * cameraUp/cameraRight: 相机的up/right方向
     * frontFactor/rightFactor/upFactor: 前进/右移/上移的权重
     *  - 比如按下了W, 则frontFactor = [是否按下W] - [是否按下S] = 1 - 0 = 1
     */
    virtual glm::vec3 computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) = 0;
};

/**
 * 允许任意方向的移动
 */
class FreeMove : public GameControlMoveStrategy {
public:
    glm::vec3 computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) override;
};

/**
 * W/S只允许在y=当前高度的平面上移动, 空格和左shift的移动方向与y轴平行
 */
class OrthoMove : public GameControlMoveStrategy {
public:
    glm::vec3 computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) override;
};

/**
 * 只允许y=0的平面上移动, 不能上下移动
 */
class PlanarMove : public GameControlMoveStrategy {
public:
    glm::vec3 computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) override;
};

#endif //GAMECONTROLMOVESTRATEGY_H

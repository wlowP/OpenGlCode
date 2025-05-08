//
// Created by ROG on 2025/4/22.
//

#include "gameControlMoveStrategy.h"

glm::vec3 FreeMove::computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) {
    glm::vec3 front = glm::cross(cameraUp, cameraRight);

    glm::vec3 direction = cameraRight * rightFactor +
                          cameraUp * upFactor +
                          front * frontFactor;

    return direction;
}

glm::vec3 OrthoMove::computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) {
    glm::vec3 front = glm::cross(cameraUp, cameraRight);
    // 忽略front的y分量以及up的x/z分量即可
    front.y = 0.0f;
    front = glm::normalize(front);

    // 只允许在y=当前高度的平面上移动
    glm::vec3 direction = cameraRight * rightFactor +
                          glm::normalize(glm::vec3(0.0f, cameraUp.y, 0.0f)) * upFactor +
                          front * frontFactor;

    return direction;
}

glm::vec3 PlanarMove::computeDirection(glm::vec3& cameraUp, glm::vec3& cameraRight, float frontFactor, float rightFactor, float upFactor) {
    // 只允许y=0的平面上移动
    glm::vec3 front = glm::cross(cameraUp, cameraRight);
    front.y = 0.0f;
    front = glm::normalize(front);

    // 只允许在y=当前高度的平面上移动
    glm::vec3 direction = cameraRight * rightFactor +
                          front * frontFactor;

    return direction;
}
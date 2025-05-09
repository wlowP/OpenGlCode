//
// Created by ROG on 2025/4/20.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "../../GLconfig/core.h"

/*
 * 相机系统类
 */
class Camera {
public:
    Camera();
    Camera(const glm::vec3& position, const glm::vec3& up, const glm::vec3& right);
    virtual ~Camera();

    // 相机位置. 默认在z轴上(0, 0, 1)
    glm::vec3 position{0.0f, 0.0f, 5.0f};
    // up方向(注意指的是相机本身本地坐标系的上方向而不是穹顶方向)
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    // 相机本身的右方向
    glm::vec3 right{1.0f, 0.0f, 0.0f};

    // 计算视图变换矩阵
    glm::mat4 getViewMatrix() const;

    // 计算投影矩阵. 有正交投影和透视投影两种, 分别在子类中实现
    virtual glm::mat4 getProjectionMatrix() const;

    // 相机缩放. 透视缩放与正交缩放的实现不同. deltaScale: 缩放比例的变化量
    virtual void zoom(float deltaScale);
};

#endif //CAMERA_H

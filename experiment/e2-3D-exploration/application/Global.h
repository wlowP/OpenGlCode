//
// Created by ROG on 2025/4/24.
//

#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>

#include "camera/perspectiveCamera.h"
#include "camera/gameCameraController.h"
#include "../GLconfig/geometry.h"
#include "shader.h"

/**
 * 存储主程序的各种全局变量
 * 加上inline修饰符防止被多个源文件包含时重复定义
 */

// 渲染的几何体对象列表
inline std::vector<GeometryInstance*> geometries;
// 封装的着色器程序对象
inline Shader* shader = nullptr;

// 相机及其控制器对象
inline PerspectiveCamera* perspectiveCamera = nullptr;
inline Camera * currentCamera = nullptr; // 当前使用的相机
inline GameCameraController* gameCameraController = nullptr;
inline CameraController* currentCameraController = nullptr; // 当前使用的相机控制器

#endif //GLOBAL_H

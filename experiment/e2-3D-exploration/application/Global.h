//
// Created by ROG on 2025/4/24.
//

#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>

#include "../GLconfig/geometry.h"

/**
 * 存储主程序的各种全局变量
 * 加上inline修饰符防止被多个源文件包含时重复定义
 */

// 渲染的几何体对象列表
inline std::vector<GeometryInstance*> geometries;
// 迷宫参数
// 迷宫尺寸(须要为奇数, 因为最外层是墙.mazeRows为Y轴方向, mazeCols为X轴方向. 则终点位置坐标为x = mazeCols - 2, y = mazeRows - 2)
inline int mazeRows = 15, mazeCols = 15;
inline glm::vec3 goalPos(mazeCols - 2, 0.5f, -mazeRows + 2); // 迷宫终点位置

#endif //GLOBAL_H

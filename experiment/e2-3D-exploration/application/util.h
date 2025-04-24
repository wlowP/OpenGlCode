//
// Created by ROG on 2025/4/24.
//

#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <vector>
#include <stack>
#include <ctime>

using namespace std;

// 定义一个坐标结构体用于保存单元位置
struct Cell {
    int x, y;
};

// 生成随机迷宫算法
// 参数: rows 行数, cols 列数, startX 起点横坐标, startY 起点纵坐标,
//       endX 终点横坐标, endY 终点纵坐标
inline vector<vector<int>> generateMaze(int rows, int cols, int startX, int startY, int endX, int endY) {
    // 为了使用递归回溯算法，确保行和列为奇数
    if (rows % 2 == 0) rows--;
    if (cols % 2 == 0) cols--;

    // 创建二维数组, 1表示墙
    vector<vector<int>> maze(rows, vector<int>(cols, 1));

    // 设定起点为0（通路）
    maze[startY][startX] = 0;

    // 随机数种子
    srand((unsigned)time(0));

    // 使用栈来实现递归回溯
    stack<Cell> cellStack;
    cellStack.push({startX, startY});

    // 定义四个方向: 右、左、下、上，步长为2，保证跨过一堵墙
    int dx[4] = {2, -2, 0, 0};
    int dy[4] = {0, 0, 2, -2};

    while (!cellStack.empty()) {
        Cell current = cellStack.top();
        vector<Cell> neighbors;

        // 搜集所有未访问的相邻单元，其间隔为2
        for (int i = 0; i < 4; i++) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];
            // 判断边界并检查是否未被访问
            if (nx >= 0 && nx < cols && ny >= 0 && ny < rows && maze[ny][nx] == 1) {
                neighbors.push_back({nx, ny});
            }
        }

        if (!neighbors.empty()) {
            // 随机选取一个邻居
            int randIndex = rand() % neighbors.size();
            Cell next = neighbors[randIndex];

            // 打通当前单元与邻居之间的墙
            int wallX = (current.x + next.x) / 2;
            int wallY = (current.y + next.y) / 2;
            maze[wallY][wallX] = 0;    // 打通墙体
            maze[next.y][next.x] = 0;  // 打通邻居单元

            // 压入邻居
            cellStack.push(next);
        }
        else {
            cellStack.pop();
        }
    }

    // 保证终点为通路
    maze[endY][endX] = 0;

    // 检测终点周围四个方向是否至少有一个通路，否则打通一个
    bool connected = false;
    int directions[4][2] = {{1,0}, {-1,0}, {0,1}, {0,-1}};
    for (auto& d : directions) {
        int ex = endX + d[0];
        int ey = endY + d[1];
        if (ex >= 0 && ex < cols && ey >= 0 && ey < rows && maze[ey][ex] == 0) {
            connected = true;
            break;
        }
    }
    // 如果终点孤立，则默认打通右侧的一堵墙（前提是边界没问题）
    if (!connected && endX + 1 < cols) {
        maze[endY][endX + 1] = 0;
    }

    return maze;
}

#endif //UTIL_H

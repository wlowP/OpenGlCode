#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include <random>
#include <cmath>

#include "Application/Application.h"
#include "ShaderConfig/shader.h"

struct Vertex {
    float x, y;
};

// 全局变量
std::vector<std::vector<Vertex>> polygons;  // 所有已完成的多个多边形
std::vector<float> polygonColors;           // 每个多边形的颜色 (R,G,B)
std::vector<Vertex> currentPolygon;         // 当前正在绘制的多边形顶点
bool isDrawingPolygon = false;              // 多边形绘制状态
Vertex tempVertex;                          // 当前鼠标位置（用于橡皮筋效果）
float lineColor[3] = {1.0f, 1.0f, 1.0f};    // 绘制时跟随鼠标的线段颜色
bool fillPolygons = true;                   // 是否填充多边形
std::random_device rd;                      // 随机数生成器
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis(0.2f, 0.9f); // 生成0.2-0.9之间的随机颜色，避免太暗或太亮
GLint colorLoc;  // 着色器中颜色变量的地址(画笔颜色)

// 拖拽相关变量
bool isDragging = false;                    // 是否正在拖拽顶点
int dragPolygonIndex = -1;                  // 被拖拽的多边形索引
int dragVertexIndex = -1;                   // 被拖拽的顶点索引
float selectionDistance = 0.025f;           // 选择顶点的距离阈值（归一化坐标）

// 新增：多边形整体拖拽和缩放相关变量
bool isDraggingPolygon = false;             // 是否正在拖拽整个多边形
bool isScalingPolygon = false;              // 是否正在缩放多边形
int selectedPolygonIndex = -1;              // 当前选中的多边形
Vertex dragStartPos;                        // 拖拽/缩放起始位置
Vertex polygonCentroid;                     // 多边形中心点

// 着色器程序对象
Shader* shader = nullptr;

// 随机生成一种颜色
void generateRandomColor(float* color) {
    color[0] = dis(gen);
    color[1] = dis(gen);
    color[2] = dis(gen);
}

// 计算点到线段的距离
float pointToLineDistance(Vertex p, Vertex a, Vertex b) {
    float lineLength = std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
    // 如果AB线段太短，直接返回点P到点A的距离
    if (lineLength < 0.00001f) return std::sqrt(std::pow(p.x - a.x, 2) + std::pow(p.y - a.y, 2));

    // 计算点p到线段ab的投影点(记为C)的系数(AP向量点乘AB向量之后除以AB的长度平方, 其绝对值等于AC与AB的长度比值)
    float t = ((p.x - a.x) * (b.x - a.x) + (p.y - a.y) * (b.y - a.y)) / (lineLength * lineLength);

    // 限制t在[0,1]范围内，确保投影点在线段上
    t = std::max(0.0f, std::min(1.0f, t));

    // 计算投影点坐标
    float projX = a.x + t * (b.x - a.x);
    float projY = a.y + t * (b.y - a.y);

    // 计算点p到投影点的距离
    return std::sqrt(std::pow(p.x - projX, 2) + std::pow(p.y - projY, 2));
}

// 计算多边形中心点
Vertex calculateCentroid(const std::vector<Vertex>& polygon) {
    Vertex centroid = {0.0f, 0.0f};
    if (polygon.empty()) return centroid;

    // 直接计算所有顶点的x, y坐标平均值
    for (const auto& v : polygon) {
        centroid.x += v.x;
        centroid.y += v.y;
    }

    centroid.x /= polygon.size();
    centroid.y /= polygon.size();
    return centroid;
}

// 检测点是否在多边形内部（射线法）
bool isPointInPolygon(Vertex point, const std::vector<Vertex>& polygon) {
    // 不考虑线段
    if (polygon.size() < 3) return false;

    int count = 0;
    const size_t n = polygon.size();

    // 点向右作射线, 循环检查每条边是否与射线相交
    for (size_t i = 0; i < n; i++) {
        Vertex p1 = polygon[i];
        Vertex p2 = polygon[(i + 1) % n];

        // 1. 先判断边是否跨过射线
        if (((p1.y > point.y) != (p2.y > point.y)) &&
            // 2. 再判断边与射线交点是否在点右边(一次函数求交点)
            (point.x < (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x)) {
            count++; // 1 && 2, 则点与多边形的第i条边相交
        }
    }
    // 奇数次相交, 则点在多边形内部
    return count % 2 == 1;
}

// 查找包含点击点的多边形
int findPolygonContainingPoint(Vertex clickPos) {
    // 直接遍历所有多边形，找到最后一个包含点击点的多边形
    for (int i = polygons.size() - 1; i >= 0; i--) {
        if (isPointInPolygon(clickPos, polygons[i])) {
            return i;
        }
    }
    return -1;
}

// 查找最近的顶点
bool findNearestVertex(Vertex clickPos, int& polygonIndex, int& vertexIndex) {
    float minDist = selectionDistance;
    bool found = false;

    // 首先检查是否点击了某个顶点
    // 遍历所有多边形的所有顶点, 如果其到鼠标点击位置的距离小于阈值(minDist = selectionDistance), 则认为选中
    for (size_t i = 0; i < polygons.size(); i++) {
        for (size_t j = 0; j < polygons[i].size(); j++) {
            float dist = std::sqrt(std::pow(clickPos.x - polygons[i][j].x, 2) +
                                   std::pow(clickPos.y - polygons[i][j].y, 2));
            if (dist < minDist) {
                minDist = dist;
                // 记录选中(正在拖拽)的多边形和顶点索引
                polygonIndex = i;
                vertexIndex = j;
                found = true;
            }
        }
    }

    // 如果没有找到顶点，检查是否点击了线段
    if (!found) {
        for (size_t i = 0; i < polygons.size(); i++) {
            for (size_t j = 0; j < polygons[i].size(); j++) {
                // 遍历所有多边形的每条边, 如果点到边的距离小于阈值(minDist = selectionDistance), 则认为选中该边
                size_t nextIdx = (j + 1) % polygons[i].size();
                float dist = pointToLineDistance(clickPos, polygons[i][j], polygons[i][nextIdx]);
                if (dist < minDist) {
                    // 在线段上创建新的顶点
                    Vertex a = polygons[i][j];
                    Vertex b = polygons[i][nextIdx];

                    // 计算投影点, 原理同#pointToLineDistance, 可以抽取为独立函数
                    float lineLength = std::sqrt(std::pow(b.x - a.x, 2) + std::pow(b.y - a.y, 2));
                    float t = ((clickPos.x - a.x) * (b.x - a.x) + (clickPos.y - a.y) * (b.y - a.y)) /
                              (lineLength * lineLength);
                    t = std::max(0.0f, std::min(1.0f, t));

                    // 在多边形的第j条边插入新顶点, 即为投影点
                    Vertex newVertex = {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y)};
                    polygons[i].insert(polygons[i].begin() + j + 1, newVertex);

                    // 记录选中(正在拖拽)的多边形和顶点索引
                    polygonIndex = i;
                    vertexIndex = j + 1; // 这是刚插入的顶点
                    return true;
                }
            }
        }
    }

    return found;
}

// 初始化着色器
void createShaderProgram() {
    shader = new Shader(
        "assets/shader/vertex.glsl",
        "assets/shader/fragment.glsl"
        );
    colorLoc = glGetUniformLocation(shader->getProgram(), "uColor");
}

// 坐标转换：屏幕坐标 -> 标准化设备坐标(NDC比例坐标)
Vertex convertCoords(GLFWwindow* window, double x, double y) {
    // std::cout << "window pos: " << x << ", " << y << std::endl;
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    // 注意NDC的坐标系是[-1, 1], 原点在中心; 而窗口坐标系是[0, width]和[0, height], 原点在左上角
    return {
            static_cast<float>(x / width * 2 - 1),
            static_cast<float>(1 - y / height * 2)
    };
}

// 窗口尺寸变化回调
void framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
    // 使用glad中的glViewport来动态更新视口的大小
    glViewport(0, 0, width, height);
}

// 鼠标回调
void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Vertex mousePos = convertCoords(window, x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            // 如果已经在绘制多边形，继续添加顶点
            if (isDrawingPolygon) {
                // 自动闭合检测（10像素阈值）
                Vertex first = currentPolygon[0];
                float dx = (mousePos.x - first.x) * 400;  // 转换为像素坐标
                float dy = (mousePos.y - first.y) * 300;

                if (currentPolygon.size() >= 3 && dx*dx + dy*dy < 100) {  // ->鼠标点击位置在以起点为圆心半径10像素的圆内
                    // 闭合多边形
                    polygons.push_back(currentPolygon);
                    // 为新的多边形生成随机颜色
                    float color[3];
                    generateRandomColor(color);
                    polygonColors.push_back(color[0]);
                    polygonColors.push_back(color[1]);
                    polygonColors.push_back(color[2]);

                    currentPolygon.clear();
                    isDrawingPolygon = false;
                } else {
                    // 添加新顶点
                    currentPolygon.push_back(mousePos);
                }
            }
                // 尝试选中并拖拽一个顶点
            else if (findNearestVertex(mousePos, dragPolygonIndex, dragVertexIndex)) {
                isDragging = true;
            }
                // 如果没有选中任何顶点或线段，开始绘制新多边形
            else {
                currentPolygon.clear();
                currentPolygon.push_back(mousePos);
                isDrawingPolygon = true;
                tempVertex = mousePos;
            }
        }
        else if (action == GLFW_RELEASE) {
            // 释放鼠标，结束拖拽
            if (isDragging) {
                isDragging = false;
                dragPolygonIndex = -1;
                dragVertexIndex = -1;
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            if (isDrawingPolygon && currentPolygon.size() >= 3) {
                // 右键强制完成当前多边形
                polygons.push_back(currentPolygon);
                // 为新的多边形生成随机颜色
                float color[3];
                generateRandomColor(color);
                polygonColors.push_back(color[0]);
                polygonColors.push_back(color[1]);
                polygonColors.push_back(color[2]);

                currentPolygon.clear();
                isDrawingPolygon = false;
            }
            else {
                // 尝试选中一个多边形
                selectedPolygonIndex = findPolygonContainingPoint(mousePos);
                if (selectedPolygonIndex >= 0) {
                    // 记录拖拽开始位置和多边形中心点
                    dragStartPos = mousePos;
                    polygonCentroid = calculateCentroid(polygons[selectedPolygonIndex]);

                    // 根据是否按下Shift键决定是拖拽还是缩放
                    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) {
                        isScalingPolygon = true;
                    } else {
                        isDraggingPolygon = true;
                    }
                }
            }
        }
        else if (action == GLFW_RELEASE) {
            // 右键释放，结束多边形操作
            isDraggingPolygon = false;
            isScalingPolygon = false;
            selectedPolygonIndex = -1;
        }
    }
}

// 键盘回调函数
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
        // 按F键切换填充模式
        fillPolygons = !fillPolygons;
    } else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        // 按C键清除所有多边形
        polygons.clear();
        polygonColors.clear();
        currentPolygon.clear();
        isDrawingPolygon = false;
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        // 按ESC退出
        APP->close();
    } else if (key == GLFW_KEY_DELETE && action == GLFW_PRESS) {
        // 按Delete键删除选中的顶点
        if (dragPolygonIndex >= 0 && dragVertexIndex >= 0) {
            // 确保删除顶点后多边形至少有3个顶点
            if (polygons[dragPolygonIndex].size() > 3) {
                polygons[dragPolygonIndex].erase(polygons[dragPolygonIndex].begin() + dragVertexIndex);
                dragVertexIndex = -1;
            }
        }
            // 删除选中的多边形
        else if (selectedPolygonIndex >= 0) {
            polygons.erase(polygons.begin() + selectedPolygonIndex);
            // 同时删除对应的颜色信息
            if (selectedPolygonIndex * 3 < polygonColors.size()) {
                polygonColors.erase(polygonColors.begin() + selectedPolygonIndex * 3,
                                    polygonColors.begin() + selectedPolygonIndex * 3 + 3);
            }
            selectedPolygonIndex = -1;
        }
    }
}

// 鼠标移动回调
void cursorPosCallback(GLFWwindow* window, double x, double y) {
    Vertex mousePos = convertCoords(window, x, y);

    // 绘制新的多边形: 更新临时顶点位置
    if (isDrawingPolygon) {
        tempVertex = mousePos;
    }
    // 拖拽顶点: 更新被拖拽顶点位置
    else if (isDragging && dragPolygonIndex >= 0 && dragVertexIndex >= 0) {
        // 更新被拖拽顶点的位置
        polygons[dragPolygonIndex][dragVertexIndex] = mousePos;
    }
    // 拖拽多边形: 更新多边形所有顶点位置
    else if (isDraggingPolygon && selectedPolygonIndex >= 0) {
        // 计算移动向量
        float dx = mousePos.x - dragStartPos.x;
        float dy = mousePos.y - dragStartPos.y;

        // 更新多边形所有顶点位置
        for (auto& vertex : polygons[selectedPolygonIndex]) {
            vertex.x += dx;
            vertex.y += dy;
        }

        // 更新拖拽起始位置
        dragStartPos = mousePos;
    }
    // 缩放多边形: 更新多边形所有顶点位置
    else if (isScalingPolygon && selectedPolygonIndex >= 0) {
        // 计算初始距离(拖拽起点到中心)与当前距离(鼠标位置到中心)
        float initialDist = std::sqrt(std::pow(dragStartPos.x - polygonCentroid.x, 2) +
                                      std::pow(dragStartPos.y - polygonCentroid.y, 2));
        float currentDist = std::sqrt(std::pow(mousePos.x - polygonCentroid.x, 2) +
                                      std::pow(mousePos.y - polygonCentroid.y, 2));

        if (initialDist > 0.0001f) {  // 避免除以零
            // 计算缩放比例(相似比)
            float scale = currentDist / initialDist;

            // 更新多边形所有顶点位置（相对于中心点缩放）
            for (auto& vertex : polygons[selectedPolygonIndex]) {
                // 顶点相对于中心点的向量
                float vx = vertex.x - polygonCentroid.x;
                float vy = vertex.y - polygonCentroid.y;

                // 缩放并更新顶点位置
                vertex.x = polygonCentroid.x + vx * scale;
                vertex.y = polygonCentroid.y + vy * scale;
            }

            // 更新拖拽起始位置
            dragStartPos = mousePos;
        }
    }
}

// 简单的三角形分割算法（扇形三角化）
std::vector<Vertex> triangulatePolygon(const std::vector<Vertex>& polygon) {
    std::vector<Vertex> triangles;
    if (polygon.size() < 3) return triangles;

    // 使用扇形三角化 - 适用于简单凸多边形
    Vertex center = polygon[0]; // 以第一个点为扇形中心

    for (size_t i = 1; i < polygon.size() - 1; i++) {
        triangles.push_back(center);
        triangles.push_back(polygon[i]);
        triangles.push_back(polygon[i + 1]);
    }

    return triangles;
}

int main() {
    system("chcp 65001 > nul"); // 设置控制台编码为UTF-8
    APP->test();

    // 初始化GLFW
    if (!APP->init(800, 600, "简单图形绘制")) {
        return -1;
    }

    // 设置回调
    APP->setOnKeyboardCallback(keyCallback);
    APP->setOnMouseClickCallback(mouseCallback);
    APP->setOnMouseMoveCallback(cursorPosCallback);
    APP->setOnResizeCallback(framebufferSizeCallback);

    // 清除颜色缓冲(设置背景色并擦除画布)
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // 创建着色器程序
    createShaderProgram();

    // 设置顶点缓冲
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);

    // 输出用户说明
    std::cout << "多边形绘制程序说明：" << std::endl;
    std::cout << "  - 左键点击添加顶点，靠近起点时自动闭合" << std::endl;
    std::cout << "  - 左键点击并拖拽可以移动顶点" << std::endl;
    std::cout << "  - 左键点击线段可以在线段上添加新顶点并拖拽" << std::endl;
    std::cout << "  - 右键点击强制完成当前多边形" << std::endl;
    std::cout << "  - 右键点击多边形并拖动可移动整个多边形" << std::endl;
    std::cout << "  - 按住Shift键+右键拖动可缩放多边形" << std::endl;
    std::cout << "  - 按F键切换填充/线框模式" << std::endl;
    std::cout << "  - 按Delete键删除选中的顶点或多边形" << std::endl;
    std::cout << "  - 按C键清除所有多边形" << std::endl;
    std::cout << "  - 按ESC键退出程序" << std::endl;

    // 渲染循环
    while (APP->update()) {
        glClear(GL_COLOR_BUFFER_BIT);

        shader->begin();
        glBindVertexArray(VAO);

        // 绘制已完成的多个多边形
        for (size_t i = 0; i < polygons.size(); i++) {
            if (polygons[i].size() < 3) continue;

            // 设置当前多边形的颜色(设置画笔颜色)
            if (i * 3 + 2 < polygonColors.size()) {
                glUniform3f(colorLoc, polygonColors[i*3], polygonColors[i*3+1], polygonColors[i*3+2]);
            } else {
                // 如果颜色数组越界，使用默认颜色
                glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
            }

            // 如果是选中的多边形，颜色略微增亮
            if (static_cast<int>(i) == selectedPolygonIndex) {
                glUniform3f(colorLoc,
                            std::min(polygonColors[i*3] * 1.3f, 1.0f),
                            std::min(polygonColors[i*3+1] * 1.3f, 1.0f),
                            std::min(polygonColors[i*3+2] * 1.3f, 1.0f));
            }

            if (fillPolygons) {
                // 填充多边形 - 使用三角形分割
                std::vector<Vertex> triangles = triangulatePolygon(polygons[i]);
                if (!triangles.empty()) {
                    glBufferData(GL_ARRAY_BUFFER, triangles.size() * sizeof(Vertex),
                                 triangles.data(), GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_TRIANGLES, 0, triangles.size());
                }

                // 绘制边框 GL_LINE_LOOP
                glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f); // 先切换"画笔颜色"为白色, 画白色边框
                glBufferData(GL_ARRAY_BUFFER, polygons[i].size() * sizeof(Vertex),
                             polygons[i].data(), GL_DYNAMIC_DRAW); // 设置绘画内容
                glDrawArrays(GL_LINE_LOOP, 0, polygons[i].size()); // 开画

                // 绘制顶点 GL_POINTS
                for (const auto& vertex : polygons[i]) {
                    Vertex point = vertex;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point, GL_DYNAMIC_DRAW);
                    glPointSize(5.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }

                // 高亮显示被选中的顶点
                if (i == dragPolygonIndex && dragVertexIndex >= 0 &&
                    dragVertexIndex < polygons[i].size()) {
                    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // 红色高亮
                    Vertex selectedPoint = polygons[i][dragVertexIndex];
                    glBufferData(GL_ARRAY_BUFFER, sizeof(selectedPoint),
                                 &selectedPoint, GL_DYNAMIC_DRAW);
                    glPointSize(8.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }

                // 如果是被选中的多边形，显示中心点
                if (static_cast<int>(i) == selectedPolygonIndex) {
                    glUniform3f(colorLoc, 1.0f, 1.0f, 0.0f); // 黄色中心点
                    Vertex centroid = calculateCentroid(polygons[i]);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(centroid), &centroid, GL_DYNAMIC_DRAW);
                    glPointSize(6.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }
            } else {
                // 只绘制线框
                glBufferData(GL_ARRAY_BUFFER, polygons[i].size() * sizeof(Vertex),
                             polygons[i].data(), GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINE_LOOP, 0, polygons[i].size());

                // 绘制顶点（小点）
                for (const auto& vertex : polygons[i]) {
                    Vertex point = vertex;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point, GL_DYNAMIC_DRAW);
                    glPointSize(5.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }

                if (i == dragPolygonIndex && dragVertexIndex >= 0 &&
                    dragVertexIndex < polygons[i].size()) {
                    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // 红色高亮
                    Vertex selectedPoint = polygons[i][dragVertexIndex];
                    glBufferData(GL_ARRAY_BUFFER, sizeof(selectedPoint),
                                 &selectedPoint, GL_DYNAMIC_DRAW);
                    glPointSize(8.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }
            }
        }

        // 绘制当前多边形(鼠标正在画的)
        if (!currentPolygon.empty()) {
            // 设置线段颜色为白色
            glUniform3f(colorLoc, lineColor[0], lineColor[1], lineColor[2]);

            // 已确定的边 GL_LINE_STRIP = GL_LINE_LOOP删去首尾相连的边
            if (currentPolygon.size() >= 2) {
                glBufferData(GL_ARRAY_BUFFER, currentPolygon.size() * sizeof(Vertex),
                             currentPolygon.data(), GL_STATIC_DRAW);
                glDrawArrays(GL_LINE_STRIP, 0, currentPolygon.size());

                // 绘制顶点
                for (const auto& vertex : currentPolygon) {
                    Vertex point = vertex;
                    glBufferData(GL_ARRAY_BUFFER, sizeof(point), &point, GL_DYNAMIC_DRAW);
                    glPointSize(5.0f);
                    glDrawArrays(GL_POINTS, 0, 1);
                }
            }

            // 橡皮筋效果线段, 跟随鼠标
            if (isDrawingPolygon) {
                Vertex rubberBand[] = {currentPolygon.back(), tempVertex};
                glBufferData(GL_ARRAY_BUFFER, sizeof(rubberBand),
                             rubberBand, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_LINES, 0, 2);
            }
        }

        shader->end();
    }

    // 清理资源
    // glDeleteVertexArrays(1, &VAO);
    // glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shader);
    APP->destroy();
    return 0;
}
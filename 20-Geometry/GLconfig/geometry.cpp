//
// Created by ROG on 2025/4/22.
//

#include "geometry.h"
#include <vector>

Geometry::Geometry() = default;
Geometry::~Geometry() {
    if (VAO) {
        glDeleteVertexArrays(1, &VAO);
    }
    if (VBOPosition) {
        glDeleteBuffers(1, &VBOPosition);
    }
    if (VBOColor) {
        glDeleteBuffers(1, &VBOColor);
    }
    if (VBOUv) {
        glDeleteBuffers(1, &VBOUv);
    }
    if (EBO) {
        glDeleteBuffers(1, &EBO);
    }
}

Geometry* Geometry::createBox(float length, float width, float height) {
    Geometry* box = new Geometry();

    float halfLength = length / 2.0f,
          halfWidth = width / 2.0f,
          halfHeight = height / 2.0f;
    // 顶点位置. 几何体中心在几何体本地坐标系的原点
    // 平面和平面之间共用的顶点不能只定义一次, 因为后续还会有法线信息
    // 注意长宽高分别对应X, Z, Y轴. 因为相机视线方向是逆Z轴
    float positions[] = {
        // 正面四个点
        -halfLength, -halfHeight, halfWidth,
         halfLength, -halfHeight, halfWidth,
         halfLength,  halfHeight, halfWidth,
        -halfLength,  halfHeight, halfWidth,
        // 背面
        -halfLength, -halfHeight, -halfWidth,
         halfLength, -halfHeight, -halfWidth,
         halfLength,  halfHeight, -halfWidth,
        -halfLength,  halfHeight, -halfWidth,
        // 左面
        -halfLength, -halfHeight, -halfWidth,
        -halfLength, -halfHeight,  halfWidth,
        -halfLength,  halfHeight,  halfWidth,
        -halfLength,  halfHeight, -halfWidth,
        // 右面
         halfLength, -halfHeight, -halfWidth,
         halfLength, -halfHeight,  halfWidth,
         halfLength,  halfHeight,  halfWidth,
         halfLength,  halfHeight, -halfWidth,
        // 顶面
        -halfLength,  halfHeight, -halfWidth,
         halfLength,  halfHeight, -halfWidth,
         halfLength,  halfHeight,  halfWidth,
        -halfLength,  halfHeight,  halfWidth,
        // 底面
        -halfLength, -halfHeight, -halfWidth,
         halfLength, -halfHeight, -halfWidth,
         halfLength, -halfHeight,  halfWidth,
        -halfLength, -halfHeight,  halfWidth,
    };
    // 颜色, 暂且默认都是白色
    float colors[] = {
        // 正面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // 背面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // 左面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // 右面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // 顶面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        // 底面
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
    };
    // 纹理坐标数据
    float uvs[] = {
        // 正面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // 背面
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        // 左面
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        // 右面
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        // 顶面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // 底面
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    // 顶点索引顺序
    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0, // 正面
        4, 5, 6, 6, 7, 4, // 背面
        8, 9, 10, 10, 11, 8, // 左面
        12, 13, 14, 14, 15, 12, // 右面
        16, 17, 18, 18, 19, 16, // 顶面
        20, 21, 22, 22, 23, 20 // 底面
    };
    // 顶点索引数量
    box->indicesCount = sizeof(indices) / sizeof(unsigned int);

    // 创建VBO
    glGenBuffers(1, &box->VBOPosition);
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glGenBuffers(1, &box->VBOColor);
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOColor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glGenBuffers(1, &box->VBOUv);
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    // 创建EBO
    glGenBuffers(1, &box->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 创建VAO
    glGenVertexArrays(1, &box->VAO);
    glBindVertexArray(box->VAO);
    // 加入属性描述信息(0 -> 位置, 1 -> 颜色, 2 -> uv坐标)
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOPosition);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOColor);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, box->VBOUv);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    // 绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box->EBO);
    // 解绑VAO
    glBindVertexArray(0);
    return box;
}

Geometry* Geometry::createSphere(float radius) {
    Geometry* sphere = new Geometry();

    // 计算出的球体顶点, uv, EBO索引数据存到vector中
    std::vector<GLfloat> positions{};
    std::vector<GLfloat> uvs{};
    std::vector<GLuint> indices{};
    // 经纬线数量(数量越多, 球体越光滑细致)
    int latitudeCount = 60;
    int longitudeCount = 60;

    // 计算球体顶点.
    // 双层循环, 计算每条纬线(外循环)与每条经线(内循环)的交点坐标以及uv坐标
    for (int i = 0; i <= latitudeCount; i++) {
        float phi = glm::pi<float>() * i / latitudeCount; // 纬线角度

        for (int j = 0; j <= longitudeCount; j++) {
            float theta = 2 * glm::pi<float>() * j / longitudeCount; // 经线角度

            // 计算球体顶点坐标(极坐标转直角坐标)
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            // uv坐标直接线性映射即可(当前经/纬线比上经/纬线总条数)
            // 算出1.0的互补数, 以防贴图翻转, 更符合直觉
            float u = 1.0f - (float)j / (float)longitudeCount;
            float v = 1.0f - (float)i / (float)latitudeCount;

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);

            uvs.push_back(u);
            uvs.push_back(v);
        }
    }

    // 计算EBO索引
    for (int i = 0; i < latitudeCount; i++) {
        for (int j = 0; j < longitudeCount; j++) {
            int p1 = i * (longitudeCount + 1) + j; // 当前点
            int p2 = p1 + longitudeCount + 1; // 下一个纬线的同一经线点
            int p3 = p1 + 1; // 当前纬线的下一个经线点
            int p4 = p2 + 1; // 下一个纬线的下一个经线点
            // 计算出两个三角形的索引(注意逆时针顺序)
            indices.push_back(p1);
            indices.push_back(p2);
            indices.push_back(p3);

            indices.push_back(p3);
            indices.push_back(p2);
            indices.push_back(p4);
        }
    }
    // 绘制的索引数量
    sphere->indicesCount = indices.size();

    // 颜色属性暂且全部设置为白色
    std::vector<GLfloat> colors(positions.size(), 1.0f);

    // 创建VBO
    glGenBuffers(1, &sphere->VBOPosition);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &sphere->VBOColor);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOColor);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glGenBuffers(1, &sphere->VBOUv);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOUv);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
    // 创建EBO
    glGenBuffers(1, &sphere->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    // 创建VAO
    glGenVertexArrays(1, &sphere->VAO);
    glBindVertexArray(sphere->VAO);
    // 加入属性描述信息(0 -> 位置, 1 -> 颜色, 2 -> uv坐标)
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOPosition);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOColor);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, sphere->VBOUv);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    // 绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->EBO);
    // 解绑VAO
    glBindVertexArray(0);

    return sphere;
}


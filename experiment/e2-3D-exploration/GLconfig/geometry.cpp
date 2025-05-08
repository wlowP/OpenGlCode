//
// Created by ROG on 2025/4/22.
//

#include <iostream>
#include <vector>
#include "geometry.h"
#include "shader.h"

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

GeometryInstance::GeometryInstance(Geometry *geometry) : geometry(geometry) {
    initBoundingSpace();
}


GeometryInstance::GeometryInstance(Geometry* geometry, const glm::vec3 position) : geometry(geometry) {
    initBoundingSpace();
    // 计算几何体实例的世界坐标中心点
    center = position;
    // 初始变换
    translate(position);
}

GeometryInstance::GeometryInstance(Geometry *geometry, float x, float y, float z) : GeometryInstance(geometry, glm::vec3(x, y, z)) {
}

void GeometryInstance::initBoundingSpace() {
    boundingSphere.center = geometry->boundingSphere.center;
    boundingSphere.radius = geometry->boundingSphere.radius;
    boundingBox.min = geometry->boundingBox.min;
    boundingBox.max = geometry->boundingBox.max;
}

void Geometry::loadTexture(const std::string& filePath) {
    texture = new TextureMipMap(filePath, 0);
}

void Geometry::bind() const {
    // 绑定VAO
    glBindVertexArray(VAO);
    // 绑定纹理对象
    if (texture) {
        texture->bindTexture();
    }
}

GeometryInstance* GeometryInstance::translate(const glm::vec3& translation) {
    // 累加变换
    translationMatrix = glm::translate(translationMatrix, translation);
    shouldUpdateCenter = true;
    shouldUpdateModelMatrix = true;
    shouldUpdateBoundingSphere = true;
    shouldUpdateBoundingBox = true;
    return this;
}
// 注意glm::rotation是围绕Y轴旋转的
// 初始的旋转和缩放变换并不会改变几何体中心点
GeometryInstance* GeometryInstance::rotate(float angle, const glm::vec3& axis) {
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(angle), axis);
    shouldUpdateCenter = true;
    shouldUpdateModelMatrix = true;
    shouldUpdateBoundingSphere = true;
    shouldUpdateBoundingBox = true;
    return this;
}
GeometryInstance* GeometryInstance::scale(const glm::vec3& scale) {
    scaleMatrix = glm::scale(scaleMatrix, scale);
    shouldUpdateCenter = true;
    shouldUpdateModelMatrix = true;
    shouldUpdateBoundingSphere = true;
    shouldUpdateBoundingBox = true;
    return this;
}
GeometryInstance *GeometryInstance::scale(float scaleX, float scaleY, float scaleZ) {
    return scale(glm::vec3(scaleX, scaleY, scaleZ));
}

void GeometryInstance::update() {
    // 将updateMatrix直接作用到模型变换矩阵上
    modelMatrix = updateMatrix * modelMatrix;
    shouldUpdateCenter = true;
    // modelMatrix已经被直接更新了, 不需要标记为脏
    // shouldUpdateModelMatrix = true;
    shouldUpdateBoundingSphere = true;
    shouldUpdateBoundingBox = true;
}

glm::mat4& GeometryInstance::getModelMatrix() {
    if (shouldUpdateModelMatrix) {
        // 计算模型变换矩阵. 注意乘法顺序是T * R * S, 则变换顺序是S * R * T
        modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
        shouldUpdateModelMatrix = false;
    }
    return modelMatrix;
}

glm::vec3& GeometryInstance::getWorldCenter() {
    if (shouldUpdateCenter) {
        // 计算几何体实例的世界坐标中心点
        center = glm::vec3(getModelMatrix() * glm::vec4(Geometry::getModelCenter(), 1.0f));
        shouldUpdateCenter = false;
    }
    return center;
}

BoundingSphere& GeometryInstance::getBoundingSphere() {
    if (shouldUpdateBoundingSphere) {
        boundingSphere.center = getWorldCenter();
        // 取最大缩放轴
        float maxScale = std::max(scaleMatrix[0][0], std::max(scaleMatrix[1][1], scaleMatrix[2][2]));
        // 计算包围球半径
        boundingSphere.radius = maxScale * geometry->boundingSphere.radius;

        shouldUpdateBoundingSphere = false;
    }

    return boundingSphere;
}
BoundingBox& GeometryInstance::getBoundingBox() {
    if (shouldUpdateBoundingBox) {
        // 计算几何体实例的AABB包围盒
        glm::mat4& modelMatrix = getModelMatrix();
        // 提取旋转缩放矩阵
        // 给mat4传递mat3构造, 会自动提取前3行3列的矩阵
        glm::mat3 rsMatrix = glm::mat3(modelMatrix);
        // 计算变换后的中心
        glm::vec3 center = getWorldCenter();
        // 获取AABB盒半长向量
        glm::vec3 halfLength = (geometry->boundingBox.max - geometry->boundingBox.min) * 0.5f ;
        // boundingBox.min = center - rsMatrix * halfLength;
        // boundingBox.max = center + rsMatrix * halfLength;
        // 4. 计算变换后的半长向量（考虑矩阵绝对值）

        // 将rsMatrix的每个分量都绝对值化
        glm::vec3 transformedExtents;
        transformedExtents.x = glm::abs(rsMatrix[0].x) * halfLength.x +
                              glm::abs(rsMatrix[1].x) * halfLength.y +
                              glm::abs(rsMatrix[2].x) * halfLength.z;

        transformedExtents.y = glm::abs(rsMatrix[0].y) * halfLength.x +
                              glm::abs(rsMatrix[1].y) * halfLength.y +
                              glm::abs(rsMatrix[2].y) * halfLength.z;

        transformedExtents.z = glm::abs(rsMatrix[0].z) * halfLength.x +
                              glm::abs(rsMatrix[1].z) * halfLength.y +
                              glm::abs(rsMatrix[2].z) * halfLength.z;

        // 5. 构建新AABB
        boundingBox.min = center - transformedExtents;
        boundingBox.max = center + transformedExtents;

        shouldUpdateBoundingBox = false;
    }

    return boundingBox;
}

// ===============================================================
// ===创建几何体的工厂方法==========================================
// =========创建的时候几何体中心都会在世界坐标系的原点================
// ===============================================================

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
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // 右面
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
        // 顶面
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
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

    // 计算包围球和AABB包围盒
    box->boundingSphere.center = glm::vec3(0.0f);
    box->boundingSphere.radius = glm::length(glm::vec3(halfLength, halfHeight, halfWidth));
    box->boundingBox.min = glm::vec3(-halfLength, -halfHeight, -halfWidth);
    box->boundingBox.max = glm::vec3(halfLength, halfHeight, halfWidth);

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

Geometry* Geometry::createSphere(float radius, int latitudeSegments, int longitudeSegments) {
    Geometry* sphere = new Geometry();

    // 计算出的球体顶点, uv, EBO索引数据存到vector中
    std::vector<GLfloat> positions{};
    std::vector<GLfloat> uvs{};
    std::vector<GLuint> indices{};

    // 计算球体顶点.
    // 双层循环, 计算每条纬线(外循环)与每条经线(内循环)的交点坐标以及uv坐标
    for (int i = 0; i <= latitudeSegments; i++) {
        float phi = glm::pi<float>() * i / latitudeSegments; // 纬线角度

        for (int j = 0; j <= longitudeSegments; j++) {
            float theta = 2 * glm::pi<float>() * j / longitudeSegments; // 经线角度

            // 计算球体顶点坐标(极坐标转直角坐标)
            float x = radius * sin(phi) * cos(theta);
            float y = radius * cos(phi);
            float z = radius * sin(phi) * sin(theta);

            // uv坐标直接线性映射即可(当前经/纬线比上经/纬线总条数)
            // 算出1.0的互补数, 以防贴图翻转, 更符合直觉
            float u = 1.0f - (float)j / (float)longitudeSegments;
            float v = 1.0f - (float)i / (float)latitudeSegments;

            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);

            uvs.push_back(u);
            uvs.push_back(v);
        }
    }

    // 计算EBO索引
    for (int i = 0; i < latitudeSegments; i++) {
        for (int j = 0; j < longitudeSegments; j++) {
            int p1 = i * (longitudeSegments + 1) + j; // 当前点
            int p2 = p1 + longitudeSegments + 1; // 下一个纬线的同一经线点
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
    std::vector<GLfloat> colors(positions.size() * 3, 1.0f);

    // 计算包围球和AABB包围盒
    sphere->boundingSphere.center = glm::vec3(0.0f);
    sphere->boundingSphere.radius = radius;
    sphere->boundingBox.min = glm::vec3(-radius, -radius, -radius);
    sphere->boundingBox.max = glm::vec3(radius, radius, radius);

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

Geometry* Geometry::createPlane(float length, float width, float segments) {
    Geometry* plane = new Geometry();

    float halfLength = length / 2.0f,
          halfWidth = width / 2.0f;

    float positions[] = {
        -halfLength, 0.0f, -halfWidth,
         halfLength, 0.0f, -halfWidth,
         halfLength, 0.0f,  halfWidth,
        -halfLength, 0.0f,  halfWidth
    };
    float colors[] = {
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f
    };
    float uvs[] = {
        0.0f, segments,
        segments, segments,
        segments, 0.0f,
        0.0f, 0.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    // 顶点索引数量
    plane->indicesCount = sizeof(indices) / sizeof(unsigned int);

    // 计算包围球和AABB包围盒
    plane->boundingSphere.center = glm::vec3(0.0f);
    plane->boundingSphere.radius = glm::length(glm::vec3(halfLength, 0.0f, halfWidth));
    // 平面增加些许厚度以免碰撞检测不到
    plane->boundingBox.min = glm::vec3(-halfLength, -0.005f, -halfWidth);
    plane->boundingBox.max = glm::vec3(halfLength, 0.005f, halfWidth);

    // 创建VBO
    glGenBuffers(1, &plane->VBOPosition);
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOPosition);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glGenBuffers(1, &plane->VBOColor);
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOColor);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glGenBuffers(1, &plane->VBOUv);
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOUv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
    // 创建EBO
    glGenBuffers(1, &plane->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 创建VAO
    glGenVertexArrays(1, &plane->VAO);
    glBindVertexArray(plane->VAO);
    // 加入属性描述信息(0 -> 位置, 1 -> 颜色, 2 -> uv坐标)
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOPosition);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOColor);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, plane->VBOUv);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    // 绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane->EBO);
    // 解绑VAO
    glBindVertexArray(0);

    return plane;
}

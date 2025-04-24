//
// Created by ROG on 2025/4/22.
//

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "core.h"
#include "TextureMipMap.h"

// 包围球
struct BoundingSphere {
    glm::vec3 center{0.0f}; // 中心点
    float radius{0.0f}; // 半径
};
// AABB包围盒
struct BoundingBox {
    glm::vec3 min{0.0f}; // 最小点
    glm::vec3 max{0.0f}; // 最大点
};
// 包围球的碰撞检测, 直接比较半径范围即可
inline bool isCollide(const BoundingSphere& a, const BoundingSphere& b) {
    return glm::length(a.center - b.center) <= (a.radius + b.radius);
}
// AABB包围盒的碰撞检测
inline bool isCollide(const BoundingBox& a, const BoundingBox& b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

class Geometry {
public:
    Geometry();
    ~Geometry();

    // 碰撞检测
    // 模型空间的包围球
    BoundingSphere boundingSphere;
    // 模型空间的AABB包围盒
    BoundingBox boundingBox;

    GLuint getVAO() const { return VAO; }
    GLuint getIndicesCount() const { return indicesCount; }

    void setPrimitiveType(const GLenum type) { primitiveType = type; }
    GLenum getPrimitiveType() const { return primitiveType; }

    // 加载纹理
    void loadTexture(const std::string& filePath);

    // 准备渲染
    void bind() const;

    // 获取几何体(模型空间)的中心位置
    static glm::vec3 getModelCenter() {
        // 几何体模型中心都默认在世界坐标系原点
        return glm::vec3(0.0f);
    }

    // 几何模型的变换矩阵都是单位阵. 要变换的话应当使用下面的几何实例类
    static glm::mat4 getModelMatrix() {
        return glm::identity<glm::mat4>();
    }

    // 创建几何体. 📌📌记得几何体中心都默认在世界坐标系原点
    // 创建长方体
    // 长宽高分别对应X, Z, Y轴. 因为相机视线方向是逆Z轴
    static Geometry* createBox(float length, float width, float height);
    // 创建球体
    // latitude/longitudeSegments: 经纬划分数量(数字越大, 球体越光滑细致)
    // longitudeCount等于经线数量, latitudeCount等于纬线数量 + 1. 实际上都等于经纬线方向上被划分的段数
    static Geometry* createSphere(float radius, int latitudeSegments, int longitudeSegments);
    // 创建平面
    // 长宽分别对应X, Z轴, segments: 平面材质被划分的次数. 当segment=2时, 纹理贴图会在平面上重复2*2=4次
    static Geometry* createPlane(float length, float width, float segments = 1.0f);

private:
    // VBO, VAO以及EBO成员
    GLuint VAO{0};
    GLuint VBOPosition{0};
    GLuint VBOColor{0};
    GLuint VBOUv{0};
    GLuint EBO{0};

    TextureMipMap* texture{nullptr}; // 纹理对象
    GLenum primitiveType{GL_TRIANGLES}; // 绘制时的图元类型(三角形, 线框等)

    // 需要绘制的EBO索引数量(注意: 不是顶点数量)
    uint32_t indicesCount{0};
};

/**
 * 场景中的几何体实例类, 数据能public的都public了
 */
class GeometryInstance {
public:
    GeometryInstance(Geometry* geometry);
    // position: 几何体中心在世界坐标系中的初始位置
    GeometryInstance(Geometry* geometry, glm::vec3 position);
    GeometryInstance(Geometry* geometry, float x, float y, float z);
    ~GeometryInstance() = default;

    Geometry* geometry{nullptr}; // 几何体模型对象

    // 每一帧都需要更新的操作变换矩阵, 例如旋转等小动画
    glm::mat4 updateMatrix{1.0f};

    // 几何体在世界坐标系中的中心点位置
    glm::vec3 center{0.0f};

    // 平移旋转缩放变换的矩阵
    glm::mat4 translationMatrix{1.0f}; // 平移矩阵
    glm::mat4 rotationMatrix{1.0f}; // 旋转矩阵
    glm::mat4 scaleMatrix{1.0f}; // 缩放矩阵

    // 模型变换操作, 可以链式调用
    GeometryInstance* translate(const glm::vec3& translation);
    GeometryInstance* rotate(float angle, const glm::vec3& axis);
    GeometryInstance* scale(const glm::vec3& scale);
    GeometryInstance* scale(float scaleX, float scaleY, float scaleZ);

    glm::mat4& getModelMatrix();
    // 获取几何体实例的世界坐标中心点
    glm::vec3& getWorldCenter();
    // 获取几何体实例的包围球/AABB包围盒
    BoundingSphere& getBoundingSphere();
    BoundingBox& getBoundingBox();

    // 是否检测碰撞
    bool detectCollision = true;

    // 需要每一帧更新的行为, 目前仅支持应用updateMatrix
    void update();

private:
    bool shouldUpdateCenter{false}; // 是否需要更新几何体实例的世界坐标中心点
    bool shouldUpdateModelMatrix{false}; // 是否需要更新模型变换矩阵
    glm::mat4 modelMatrix{1.0f}; // 模型变换矩阵

    // 几何体实例的包围球/AABB包围盒
    BoundingSphere boundingSphere;
    BoundingBox boundingBox;

    // 创建实例时初始化包围球/AABB包围盒
    void initBoundingSpace();
};

#endif //GEOMETRY_H

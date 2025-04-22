//
// Created by ROG on 2025/4/22.
//

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "core.h"

class Geometry {
public:
    Geometry();
    ~Geometry();

    // 创建几何体
    // 长宽高分别对应X, Z, Y轴. 因为相机视线方向是逆Z轴
    static Geometry* createBox(float length, float width, float height);
    static Geometry* createSphere(float radius);

    GLuint getVAO() const { return VAO; }
    GLuint getIndicesCount() const { return indicesCount; }

private:
    // VBO, VAO以及EBO成员
    GLuint VAO{0};
    GLuint VBOPosition{0};
    GLuint VBOColor{0};
    GLuint VBOUv{0};
    GLuint EBO{0};

    // 需要绘制的EBO索引数量(注意: 不是顶点数量)
    uint32_t indicesCount{0};
};

#endif //GEOMETRY_H

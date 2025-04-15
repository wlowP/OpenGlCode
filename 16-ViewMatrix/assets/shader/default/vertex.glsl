#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;

// 模型变换矩阵(model matrix)
uniform mat4 transform;
// 视图变换矩阵(view matrix)
uniform mat4 viewMatrix;

void main() {
    // 变换顶点坐标
    vec4 position = transform * vec4(aPos, 1.0);
    // 视图变换(将世界坐标系转换为视图坐标系)
    position = viewMatrix * position;

    gl_Position = position;
    color = aColor;
    uvTexCoord = aTexCoord;
}
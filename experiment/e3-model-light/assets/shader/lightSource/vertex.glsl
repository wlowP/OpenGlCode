#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;

// 模型变换矩阵(model matrix)
uniform mat4 model;
// 视图变换矩阵(view matrix)
uniform mat4 viewMatrix;
// 投影变换矩阵(projection matrix)
uniform mat4 projectionMatrix;

void main() {
    // 变换顶点坐标
    vec4 position = vec4(aPos, 1.0);
    // 变换顺序: 模型变换 -> 视图变换 -> 投影变换
    position = projectionMatrix * viewMatrix * model * position;

    // 输出的position为裁剪空间坐标, 会经过透视除法. 透视投影时, w分量一般不等于1
    gl_Position = position;
    color = aColor;
    uvTexCoord = aTexCoord;
}
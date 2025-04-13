#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;

// 变换矩阵
uniform mat4 transform;

void main() {
    // 变换顶点坐标
    vec4 transformed = transform * vec4(aPos, 1.0);

    gl_Position = transformed;
    color = aColor;
    uvTexCoord = aTexCoord;
}
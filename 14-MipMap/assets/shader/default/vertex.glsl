#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;

// 多边形形状随时间变小, 以便观察不同的 mipmap 级别
uniform float uTime;

void main() {
    // 多边形相对于中心原点缩小
    vec3 shrink = aPos * (1.0 / uTime);
    gl_Position = vec4(shrink, 1.0);
    color = aColor;
    uvTexCoord = aTexCoord;
}
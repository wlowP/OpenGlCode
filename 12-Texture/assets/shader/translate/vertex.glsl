#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform float uTime;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    color = aColor;
    uvTexCoord = aTexCoord + vec2(uTime);
}
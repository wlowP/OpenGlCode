#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec3 color;
uniform float uTime;

void main()
{
    // 使用NDC顶点坐标. 加上正弦变换实现往复运动
    gl_Position = vec4(aPos.x + 0.5 * sin(uTime), aPos.y + 0.5 * cos(uTime), aPos.z, 1.0);
    // color = aColor;

    // 使用正弦函数实现颜色渐变效果
    color = aColor * (0.5f * sin(uTime) + 0.5f);
}
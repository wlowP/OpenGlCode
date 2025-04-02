#version 460 core
out vec4 FragColor;

in vec3 color;
// 纹理坐标
in vec2 uvTexCoord;

// 采样器
uniform sampler2D sampler;

void main() {
   // 像素颜色就不是顶点颜色而是纹理图片中的颜色了
   FragColor = texture(sampler, uvTexCoord);
}
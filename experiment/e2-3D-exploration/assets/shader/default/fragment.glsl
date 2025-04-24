#version 460 core
out vec4 FragColor;

in vec3 color;
// 纹理坐标
in vec2 uvTexCoord;

// 采样器. 其值代表纹理单元的索引(sampler = 0 -> 从0号纹理单元中采样)
uniform sampler2D sampler;

void main() {
    FragColor = texture(sampler, uvTexCoord);
}
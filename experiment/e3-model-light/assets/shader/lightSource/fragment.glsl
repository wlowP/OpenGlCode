#version 460 core
out vec4 FragColor;

in vec3 color;
// 纹理坐标
in vec2 uvTexCoord;

// 采样器. 其值代表纹理单元的索引(sampler = 0 -> 从0号纹理单元中采样)
uniform sampler2D sampler;
// 是否应用纹理坐标
uniform bool useTexture = true;

void main() {
    // 根据useTexture判断是否加载纹理
    if (useTexture) {
        FragColor = texture(sampler, uvTexCoord);
    } else {
        FragColor = vec4(color, 1.0f);
    }
}
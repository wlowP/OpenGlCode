#version 460 core
out vec4 FragColor;

in vec3 color;
// 纹理坐标
in vec2 uvTexCoord;

// 采样器. 其值代表纹理单元的索引(sampler = 0 -> 从0号纹理单元中采样)
uniform sampler2D samplerGrass;
uniform sampler2D samplerSoil;
uniform sampler2D samplerNoise;

void main() {
   // texture()返回的是一个vec4类型的rgba颜色值
   vec4 grassColor = texture(samplerGrass, uvTexCoord);
   vec4 soilColor = texture(samplerSoil, uvTexCoord);
   // 读取黑白噪声纹理, 作为草地和土壤的混合因子
   // 因为纹理的rgba分量都相同, 所以只取其中一个分量即可
   float noiseFactor = texture(samplerNoise, uvTexCoord).r;
   // 混合草地和土壤的颜色, 使用噪声因子
   // FragColor = grassColor * noiseFactor + soilColor * (1.0f - noiseFactor);
   // OpenGL内置了混合函数, 直接使用. 第三个参数w代表第二个颜色的权重, 于是第一个颜色就是1-w权重
   FragColor = mix(soilColor, grassColor, noiseFactor);
}
#version 460 core
out vec4 FragColor;

in vec3 color;
// 纹理坐标
in vec2 uvTexCoord;
in vec3 normal;
in vec3 fragPos;

// 采样器. 其值代表纹理单元的索引(sampler = 0 -> 从0号纹理单元中采样)
uniform sampler2D sampler;
// 是否应用纹理坐标
uniform bool useTexture = true;
// 环境光颜色, 默认为白色
uniform vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
// 光源位置
uniform vec3 lightPosition;
// 观察者位置, 用于计算镜面光照
uniform vec3 viewPosition;

void main() {
    // 根据useTexture判断是否加载纹理
    if (useTexture) {
        FragColor = texture(sampler, uvTexCoord);
    } else {
        // 加载光照. blinn-phong模型(物体颜色直接取自顶点颜色VAO)
        // ===1. 环境光
        float ambientStrength = 0.1f;
        vec3 ambient = ambientStrength * lightColor;

        // ===2. 漫反射
        // 先计算光照方向
        // 📌📌如果进行了不等比缩放, 法线方向会被破坏造成光照异常, 需要额外引入法线矩阵
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightPosition - fragPos);
        // 计算漫反射分量(朗伯余弦定律)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // ===3. 镜面反射
        float specularStrength = 0.5f;
        vec3 viewDir = normalize(viewPosition - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 48);
        vec3 specular = specularStrength * spec * lightColor;

        vec3 result = (ambient + diffuse + specular) * color;
        FragColor = vec4(result, 1.0);
    }
}
#version 460 core
out vec4 FragColor;

// 物体的材质属性
struct Material {
    vec3 ambient; // 环境光照下的颜色
    vec3 diffuse; // 漫反射光照下的颜色
    // ambient 和 diffuse 一般就是物体本身颜色
    vec3 specular; // 镜面高光的颜色
    float shininess; // 镜面高光的散射/半径. 值越大, 散射越小(集中)
};
// 光源属性. 三种光强度也可以看做三种颜色, 替代了单一设置一种光照颜色
struct LightSource {
    vec3 position;
    vec3 ambient; // 环境光强度
    vec3 diffuse; // 漫反射光强度
    vec3 specular; // 镜面光强度
};

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
uniform Material material;
uniform LightSource lightSource;

void main() {
    // 根据useTexture判断是否加载纹理
    if (useTexture) {
        FragColor = texture(sampler, uvTexCoord);
    } else {
        // 加载光照. blinn-phong模型(📌📌物体颜色取自几何类的Material成员)
        // ===1. 环境光
        vec3 ambient = lightSource.ambient * material.ambient;

        // ===2. 漫反射
        // 先计算光照方向
        // 📌📌如果进行了不等比缩放, 法线方向会被破坏造成光照异常, 需要额外引入法线矩阵
        vec3 norm = normalize(normal);
        vec3 lightDir = normalize(lightPosition - fragPos);
        // 计算漫反射分量(朗伯余弦定律)
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lightSource.diffuse * (diff * material.diffuse);

        // ===3. 镜面反射
        vec3 viewDir = normalize(viewPosition - fragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lightSource.specular * (spec * material.specular);

        // 因为物体的颜色属性已经包含进Material结构体中了, 所以这里不需要再乘以物体顶点颜色
        // vec3 result = (ambient + diffuse + specular) * color;
        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}
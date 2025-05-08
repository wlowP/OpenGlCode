#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec3 color;
// 输出纹理坐标到片段着色器
out vec2 uvTexCoord;
out vec3 normal;
// 片元的世界位置. 用于在片段着色器中计算光照
out vec3 fragPos;

// 模型变换矩阵(model matrix)
uniform mat4 model;
// 视图变换矩阵(view matrix)
uniform mat4 viewMatrix;
// 投影变换矩阵(projection matrix)
uniform mat4 projectionMatrix;
// 法线矩阵. 用于将法向从模型空间变换到世界空间(相当于模型矩阵左上角3x3部分的逆矩阵的转置矩阵)
uniform mat3 normalMatrix;

void main() {
    // 变换顶点坐标
    vec4 position = vec4(aPos, 1.0);
    // 变换顺序: 模型变换 -> 视图变换 -> 投影变换
    position = projectionMatrix * viewMatrix * model * position;

    // 输出的position为裁剪空间坐标, 会经过透视除法. 透视投影时, w分量一般不等于1
    gl_Position = position;
    color = aColor;
    uvTexCoord = aTexCoord;
    fragPos = vec3(model * vec4(aPos, 1.0));
    // 模型变换也要作用于法向上, 只不过模型矩阵要先处理为法线矩阵
    normal = normalMatrix * aNormal;
}
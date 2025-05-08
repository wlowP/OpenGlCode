//
// Created by ROG on 2025/5/8.
//

#ifndef MESH_H
#define MESH_H

#include <vector>

#include "core.h"
#include "shader.h"
#include "assimp/types.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
struct TextureInfo {
    GLuint id;
    // 纹理的类型. (漫反射纹理, 镜面反射纹理)
    std::string type;
    // 纹理的路径. 用于判断是否已经加载过对应纹理
    aiString path;
};

/**
 * 网格类. 用于转化, 存储assimp读取的模型数据以便OpenGL识别和绘制
 */
class Mesh {
public:
    /*  网格数据  */
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureInfo> textures;
    /*  函数  */
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureInfo>& textures);
    void draw(const Shader* shader) const;
private:
    /*  渲染数据  */
    unsigned int VAO, VBO, EBO;
    /*  函数  */
    void setupMesh();
};
#endif //MESH_H

//
// Created by ROG on 2025/5/8.
//

#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../GLconfig/mesh.h"
#include "../GLconfig/shader.h"

class Model {
public:
    /*  函数   */
    Model(const char* path);
    Model(const char* path, int iteration, float lambda, float mu);
    void draw(const Shader* shader) const;
    void smooth();
private:
    /*  模型数据  */
    std::vector<TextureInfo> loadedTextures;
    std::vector<Mesh> meshes;
    std::string directory;
    std::string path; // 文件绝对路径
    // Laplacian smoothing参数
    int iteration{1};
    float lambda{0.33f};
    float mu{-0.34f};

    /*  函数   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureInfo> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif //MODEL_H

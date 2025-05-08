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
    void draw(const Shader* shader) const;
private:
    /*  模型数据  */
    std::vector<TextureInfo> loadedTextures;
    std::vector<Mesh> meshes;
    std::string directory;
    /*  函数   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureInfo> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif //MODEL_H

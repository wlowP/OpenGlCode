//
// Created by ROG on 2025/5/8.
//

#include <iostream>

#include "model.h"
#include "../GLconfig/Texture.h"

Model::Model(const char* path) {
    loadModel(path);
}

void Model::draw(const Shader* shader) const {
    for(auto & mesh : meshes)
        mesh.draw(shader);
}

void Model::loadModel(std::string path) {
    Assimp::Importer import;
    /*
     * 读取模型文件. 第二个参数用于配置读取时的处理选项.
     *  aiProcess_Triangulate: 如果模型不是全部由三角形组成, 则将其中所有图元转变为三角形
     *  aiProcess_FlipUVs: 读取时反转纹理坐标的y轴
     */
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // 处理节点所有的网格（如果有的话）
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // 接下来对它的子节点重复这一过程
    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<TextureInfo> textures;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++){
        Vertex vertex{};
        // 处理顶点位置、法线和纹理坐标
        glm::vec3 temp;
        temp.x = mesh->mVertices[i].x;
        temp.y = mesh->mVertices[i].y;
        temp.z = mesh->mVertices[i].z;
        vertex.position = temp;

        temp.x = mesh->mNormals[i].x;
        temp.y = mesh->mNormals[i].y;
        temp.z = mesh->mNormals[i].z;
        vertex.normal = temp;

        // 先判断网格是否有纹理坐标
        if(mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.uv = vec;
        }
        else
            vertex.uv = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }
    // 处理索引
    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // 处理材质
    if(mesh->mMaterialIndex >= 0){
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<TextureInfo> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<TextureInfo> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<TextureInfo> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    std::vector<TextureInfo> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(auto & loadedTexture : loadedTextures) {
            // 比较loadedTexture的path与当前纹理路径是否相同, 如果相同则跳过
            if(std::strcmp(loadedTexture.path.C_Str(), str.C_Str()) == 0) {
                textures.push_back(loadedTexture);
                skip = true;
                break;
            }
        }
        if(!skip) {
            // 如果纹理还没有被加载，则加载它
            TextureInfo texture;

            texture.id = Texture::TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            loadedTextures.push_back(texture); // 添加到已加载的纹理中
        }
    }
    return textures;
}


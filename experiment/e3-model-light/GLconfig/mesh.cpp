//
// Created by ROG on 2025/5/8.
//

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <map>

#include "mesh.h"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<TextureInfo>& textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    // 顶点法线
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glBindVertexArray(0);
}

void Mesh::draw(const Shader* shader) const {
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    // 绑定模型中的多个纹理对象
    for(unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        shader->setInt("material." + name + number, i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::smoothWithTaubin(int iterations, float lambda, float mu) {
    detectBoundaryVertices(); // 检测边界顶点

    std::unordered_map<unsigned int, std::vector<unsigned int>> adjacencyList;

    // 构建邻接表（使用unordered_set避免重复邻居）
    std::unordered_map<unsigned int, std::unordered_set<unsigned int>> adjacencySet;
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int v0 = indices[i], v1 = indices[i+1], v2 = indices[i+2];
        adjacencySet[v0].insert(v1);
        adjacencySet[v0].insert(v2);
        adjacencySet[v1].insert(v0);
        adjacencySet[v1].insert(v2);
        adjacencySet[v2].insert(v0);
        adjacencySet[v2].insert(v1);
    }
    for (auto& [v, neighbors] : adjacencySet) {
        adjacencyList[v] = std::vector<unsigned int>(neighbors.begin(), neighbors.end());
    }

    // Taubin平滑迭代
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<glm::vec3> originalPositions;
        for (const auto& v : vertices) originalPositions.push_back(v.position);

        // 收缩步骤（跳过边界顶点）
        std::vector<glm::vec3> contractedPositions = originalPositions;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (boundaryVertices.count(i) || !adjacencyList.count(i)) continue;

            glm::vec3 avg(0.0f);
            for (auto n : adjacencyList[i]) avg += originalPositions[n];
            avg /= adjacencyList[i].size();
            contractedPositions[i] = originalPositions[i] + lambda * (avg - originalPositions[i]);
        }

        // 膨胀步骤（跳过边界顶点）
        std::vector<glm::vec3> expandedPositions = contractedPositions;
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (boundaryVertices.count(i) || !adjacencyList.count(i)) continue;

            glm::vec3 avg(0.0f);
            for (auto n : adjacencyList[i]) avg += contractedPositions[n];
            avg /= adjacencyList[i].size();
            expandedPositions[i] = contractedPositions[i] + mu * (avg - contractedPositions[i]);
        }

        // 更新顶点位置
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertices[i].position = expandedPositions[i];
        }
    }

    recalculateNormals();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void Mesh::recalculateNormals() {
    // 清零法线
    for (auto& v : vertices) v.normal = glm::vec3(0.0f);

    // 计算面法线并累加
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i], i1 = indices[i+1], i2 = indices[i+2];
        glm::vec3 p0 = vertices[i0].position;
        glm::vec3 p1 = vertices[i1].position;
        glm::vec3 p2 = vertices[i2].position;

        glm::vec3 edge1 = p1 - p0;
        glm::vec3 edge2 = p2 - p0;
        glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

        vertices[i0].normal += normal;
        vertices[i1].normal += normal;
        vertices[i2].normal += normal;
    }

    // 单位化法线
    for (auto& v : vertices) {
        if (glm::length(v.normal) > 0.0f) {
            v.normal = glm::normalize(v.normal);
        } else {
            v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // 避免零向量
        }
    }
}

void Mesh::detectBoundaryVertices() {
    std::map<std::pair<unsigned int, unsigned int>, int> edgeCount;

    // 遍历所有三角形，统计每条边的出现次数
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int v0 = indices[i], v1 = indices[i+1], v2 = indices[i+2];

        // 生成规范化的边表示（小顶点在前）
        auto edge1 = (v0 < v1) ? std::make_pair(v0, v1) : std::make_pair(v1, v0);
        auto edge2 = (v1 < v2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
        auto edge3 = (v2 < v0) ? std::make_pair(v2, v0) : std::make_pair(v0, v2);

        edgeCount[edge1]++;
        edgeCount[edge2]++;
        edgeCount[edge3]++;
    }

    // 标记只出现一次的边为边界边，其顶点为边界顶点
    boundaryVertices.clear();
    for (const auto& [edge, count] : edgeCount) {
        if (count == 1) {
            boundaryVertices.insert(edge.first);
            boundaryVertices.insert(edge.second);
        }
    }
}
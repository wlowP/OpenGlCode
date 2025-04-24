#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <iostream>
#include <vector>
#include <cmath>

// 窗口设置
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1600;

// 相机/玩家设置
glm::vec3 cameraPos = glm::vec3(0.0f, 1.5f, 10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float cameraSpeed = 0.5f;

// 鼠标控制
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = SCR_WIDTH / 2.0;
float lastY = SCR_HEIGHT / 2.0;
bool firstMouse = true;
float mouseSensitivity = 0.1f;
struct Obstacle {
    glm::vec3 position;
    float radius;
};
std::vector<Obstacle> obstacles;
const int NUM_OBSTACLES = 2;

// 游戏目标
glm::vec3 goalPos = glm::vec3(0.0f, 0.0f, -50.0f);
float goalRadius = 1.0f;
bool reachedGoal = false;

// 帧时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// 地形设置
const int TERRAIN_SIZE = 100                                                                                                                                                                                                                                                                                                                                                                   ;
const float TERRAIN_SCALE = 0.0f;  // 高度缩放因子
const float TERRAIN_HEIGHTMAP_SCALE =0.0f; // 高度图缩放因子
std::vector<float> heightMap;  // 存储地形高度

// 生成地形高度图
void generateHeightMap() {
    // 初始化随机种子
    srand(42);  // 使用固定种子以获得可重复的结果

    heightMap.resize((TERRAIN_SIZE+1) * (TERRAIN_SIZE+1));

    // 使用简单的噪声函数生成高度图
    for (int z = 0; z <= TERRAIN_SIZE; z++) {
        for (int x = 0; x <= TERRAIN_SIZE; x++) {
            float xf = (float)x  / TERRAIN_SIZE;
            float zf = (float)z / TERRAIN_SIZE;

            // 多层次噪声，创建更自然的地形
            float height = 0.0f;
            height += sin(xf * 5.0f) * cos(zf * 5.0f) * 0.5f;
            height += sin(xf * 12.0f) * cos(zf * 12.0f) * 0.25f;
            height += sin(xf * 25.0f) * cos(zf * 25.0f) * 0.125f;

            // 添加一些随机变化
            height += ((float)rand() / RAND_MAX) * 0.05f;

            // 缩放高度并存储
            heightMap[z * (TERRAIN_SIZE+1) + x] = height * TERRAIN_HEIGHTMAP_SCALE;
        }
    }
}

// 根据坐标获取高度值
float getHeight(float x, float z) {
    // 将世界坐标转换为地形坐标
    float terrainX = (x + TERRAIN_SIZE/2) / TERRAIN_SCALE;
    float terrainZ = (z + TERRAIN_SIZE/2) / TERRAIN_SCALE;

    // 确保坐标在有效范围内
    if (terrainX < 0) terrainX = 0;
    if (terrainZ < 0) terrainZ = 0;
    if (terrainX > TERRAIN_SIZE) terrainX = TERRAIN_SIZE;
    if (terrainZ > TERRAIN_SIZE) terrainZ = TERRAIN_SIZE;

    // 计算网格单元索引
    int gridX = (int)terrainX;
    int gridZ = (int)terrainZ;

    // 计算网格内的相对位置 (用于插值)
    float xCoord = terrainX - gridX;
    float zCoord = terrainZ - gridZ;

    // 确保索引不会越界
    if (gridX >= TERRAIN_SIZE) gridX = TERRAIN_SIZE - 1;
    if (gridZ >= TERRAIN_SIZE) gridZ = TERRAIN_SIZE - 1;

    // 获取四个最近点的高度
    float h1 = heightMap[gridZ * (TERRAIN_SIZE+1) + gridX];       // 左上
    float h2 = heightMap[gridZ * (TERRAIN_SIZE+1) + (gridX+1)];   // 右上
    float h3 = heightMap[(gridZ+1) * (TERRAIN_SIZE+1) + gridX];   // 左下
    float h4 = heightMap[(gridZ+1) * (TERRAIN_SIZE+1) + (gridX+1)]; // 右下

    // 双线性插值计算确切高度
    float h = h1 * (1-xCoord) * (1-zCoord) +
              h2 * xCoord * (1-zCoord) +
              h3 * (1-xCoord) * zCoord +
              h4 * xCoord * zCoord;

    return h;
}
// 着色器程序（顶点着色器）
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

// 片段着色器
const char *fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture1;
uniform vec3 objectColor;
uniform bool useTexture;
uniform bool isSkybox;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    if (isSkybox) {
        FragColor = texture(texture1, TexCoord);
    } else if (useTexture) {
        // 环境光
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);

        // 漫反射
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

        // 镜面反射
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

        vec3 lighting = (ambient + diffuse + specular);
        FragColor = vec4(lighting, 1.0) * texture(texture1, TexCoord);
    } else {
        FragColor = vec4(objectColor, 1.0);
    }
}
)";

// 天空盒顶点着色器
const char *skyboxVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoord = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
)";

// 天空盒片段着色器
const char *skyboxFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 TexCoord;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, TexCoord);
}
)";
// 生成随机障碍物
void generateObstacles() {
    // 确保每次运行生成相同的障碍物
    srand(42);

    // 在起点和终点之间创建一条曲折的路径
    glm::vec3 pathStart = cameraPos;
    glm::vec3 pathEnd = goalPos;

    // 在路径周围添加障碍物
    for (int i = 0; i < NUM_OBSTACLES; i++) {
        Obstacle obstacle;

        // 在起点和终点之间分布障碍物
        float progress = (float)i / NUM_OBSTACLES;

        // 创建一条S形路径
        float pathX = pathStart.x + (pathEnd.x - pathStart.x) * progress;
        float pathZ = pathStart.z + (pathEnd.z - pathStart.z) * progress;

        // 添加一些曲折 - 使用正弦函数创建S形
        pathX += sin(progress * 4 * M_PI) * 15.0f;

        // 在路径两侧放置障碍物
        float sideOffset = (i % 2 == 0) ? 4.0f : -4.0f;
        float x = pathX + sideOffset + ((float)rand() / RAND_MAX - 0.5f) * 3.0f;
        float z = pathZ + ((float)rand() / RAND_MAX - 0.5f) * 5.0f;

        // 设置障碍物位置和半径
        obstacle.position = glm::vec3(x, getHeight(x, z) + 1.0f, z);
        obstacle.radius = 1.5f + (rand() % 100) / 100.0f;  // 稍微增大障碍物

        obstacles.push_back(obstacle);
    }

    // 额外添加一些完全随机的障碍物
    for (int i = 0; i < NUM_OBSTACLES/2; i++) {
        Obstacle obstacle;

        float x, z;
        bool validPosition = false;

        while (!validPosition) {
            x = (float)(rand() % TERRAIN_SIZE - TERRAIN_SIZE/2) * TERRAIN_SCALE;
            z = (float)(rand() % TERRAIN_SIZE - TERRAIN_SIZE/2) * TERRAIN_SCALE;

            // 确保与起点和终点有一定距离
            float distToStart = glm::distance(glm::vec2(x, z), glm::vec2(cameraPos.x, cameraPos.z));
            float distToGoal = glm::distance(glm::vec2(x, z), glm::vec2(goalPos.x, goalPos.z));

            // 确保不会挡住主路径
            if (distToStart > 8.0f && distToGoal > 8.0f) {
                validPosition = true;

                // 额外检查不会完全阻塞路径
                for (const auto& obs : obstacles) {
                    if (glm::distance(glm::vec2(x, z), glm::vec2(obs.position.x, obs.position.z)) < 5.0f) {
                        validPosition = false;
                        break;
                    }
                }
            }
        }

        // 设置障碍物位置和半径
        obstacle.position = glm::vec3(x, getHeight(x, z) + 1.0f, z);
        obstacle.radius = 1.0f + (rand() % 100) / 100.0f;

        obstacles.push_back(obstacle);
    }
}

// Francis Bourassa
// 处理键盘输入
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeedAdjusted = cameraSpeed * deltaTime * 60.0f;

    glm::vec3 originalPos = cameraPos;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        cameraPos += forward * cameraSpeedAdjusted;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 forward = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        cameraPos -= forward * cameraSpeedAdjusted;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        cameraPos -= right * cameraSpeedAdjusted;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 right = glm::normalize(glm::cross(cameraFront, cameraUp));
        cameraPos += right * cameraSpeedAdjusted;
    }

    // 计算玩家在地形上的高度（略微悬浮在地形上方）
    float terrainHeight = getHeight(cameraPos.x, cameraPos.z);
    cameraPos.y = terrainHeight + 1.5f;  // 1.5是玩家视点高度

    // 边界检测 - 防止玩家走出地形
    if (abs(cameraPos.x) > TERRAIN_SIZE/2 * TERRAIN_SCALE ||
        abs(cameraPos.z) > TERRAIN_SIZE/2 * TERRAIN_SCALE) {
        cameraPos = originalPos;  // 如果超出边界，回到原位置
    }

    // 检测与障碍物的碰撞
    bool collisionDetected = false;
    for (const auto& obstacle : obstacles) {
        float distToObstacle = glm::distance(
                glm::vec2(cameraPos.x, cameraPos.z),
                glm::vec2(obstacle.position.x, obstacle.position.z)
        );

        // 玩家半径设为0.5
        float playerRadius = 0.5f;
        if (distToObstacle < (obstacle.radius + playerRadius)) {
            collisionDetected = true;
            break;
        }
    }

    if (collisionDetected) {
        cameraPos = originalPos;  // 如果碰到障碍物，回到原位置
    }

    // 检查是否到达目标
    float distToGoal = glm::distance(glm::vec2(cameraPos.x, cameraPos.z), glm::vec2(goalPos.x, goalPos.z));
    if (distToGoal < goalRadius) {
        reachedGoal = true;
        std::cout << "恭喜！你到达了目标点！游戏结束！" << std::endl;
    }
}

// 处理鼠标移动
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // 限制仰角，防止翻转
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 创建着色器程序
unsigned int createShaderProgram(const char *vertexSrc, const char *fragmentSrc) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    // 检查顶点着色器是否编译成功
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "顶点着色器编译失败:\n" << infoLog << std::endl;
    }

    // 片段着色器
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "片段着色器编译失败:\n" << infoLog << std::endl;
    }

    // 链接着色器程序
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "着色器程序链接失败:\n" << infoLog << std::endl;
    }

    // 删除着色器
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// 设置障碍物顶点数据（使用立方体表示）- 修改纹理坐标使其更好地覆盖
void setupCubeVertices(unsigned int &VAO, unsigned int &VBO) {
    float vertices[] = {
            // 位置              // 纹理     // 法线
            // 纹理坐标修改为适当的值，确保纹理覆盖整个面
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f,  0.0f,

            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 法线属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// 设置地形顶点数据
void setupTerrainVertices(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // 生成顶点
    for (int z = 0; z <= TERRAIN_SIZE; z++) {
        for (int x = 0; x <= TERRAIN_SIZE; x++) {
            // 计算实际位置 (缩放并居中)
            float posX = (x - TERRAIN_SIZE/2) * TERRAIN_SCALE;
            float posZ = (z - TERRAIN_SIZE/2) * TERRAIN_SCALE;
            float posY = heightMap[z * (TERRAIN_SIZE+1) + x];  // 使用高度图

            float texU = (float)x / (TERRAIN_SIZE / 200.0f);
            float texV = (float)z / (TERRAIN_SIZE / 200.0f);

            // 计算法线代码保持不变
            float hL = x > 0 ? heightMap[z * (TERRAIN_SIZE+1) + (x-1)] : posY;
            float hR = x < TERRAIN_SIZE ? heightMap[z * (TERRAIN_SIZE+1) + (x+1)] : posY;
            float hD = z > 0 ? heightMap[(z-1) * (TERRAIN_SIZE+1) + x] : posY;
            float hU = z < TERRAIN_SIZE ? heightMap[(z+1) * (TERRAIN_SIZE+1) + x] : posY;

            // 计算X和Z方向的斜率, 生成法线向量
            glm::vec3 normal = glm::normalize(glm::vec3(hL - hR, 2.0f, hD - hU));

            // 添加顶点数据: 位置, 纹理坐标, 法线
            vertices.push_back(posX);
            vertices.push_back(posY);
            vertices.push_back(posZ);
            vertices.push_back(texU);
            vertices.push_back(texV);
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }


    // 生成索引 (三角形带)
    for (int z = 0; z < TERRAIN_SIZE; z++) {
        for (int x = 0; x < TERRAIN_SIZE; x++) {
            // 计算当前网格的顶点索引
            int topLeft = z * (TERRAIN_SIZE + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (TERRAIN_SIZE + 1) + x;
            int bottomRight = bottomLeft + 1;

            // 添加两个三角形
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 法线属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// 设置目标顶点数据（用球体表示）
void setupSphereVertices(unsigned int &VAO, unsigned int &VBO, unsigned int &EBO) {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    const int sectorCount = 36;
    const int stackCount = 18;
    const float radius = 1.0f;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f;
    float s, t;

    float sectorStep = 2 * M_PI / sectorCount;
    float stackStep = M_PI / stackCount;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stackCount; ++i) {
        stackAngle = M_PI / 2 - i * stackStep;
        xy = radius * cos(stackAngle);
        z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            sectorAngle = j * sectorStep;

            // 顶点位置
            x = xy * cos(sectorAngle);
            y = xy * sin(sectorAngle);

            // 法向量
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            // 纹理坐标
            s = (float) j / sectorCount;
            t = (float) i / stackCount;

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(s);
            vertices.push_back(t);
            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    // 生成索引
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != stackCount - 1) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 法线属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

// 设置天空盒顶点数据
void setupSkyboxVertices(unsigned int &VAO, unsigned int &VBO) {
    float skyboxVertices[] = {
            // 位置
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
}

// 创建一个默认纹理（当文件不存在时使用）
unsigned int createDefaultTexture(const glm::vec3 &color) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 创建一个默认颜色纹理
    unsigned char data[3] = {
            static_cast<unsigned char>(color.r * 255),
            static_cast<unsigned char>(color.g * 255),
            static_cast<unsigned char>(color.b * 255)
    };

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    return texture;
}

// 加载天空盒纹理
unsigned int loadSkybox() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 创建一个简单的天空颜色（蓝色）
    unsigned char skyColor[3] = {135, 206, 235}; // 淡蓝色

    // 创建一个1x1像素的纹理，表示天空
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, skyColor);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

// 主函数
int main() {
    system("chcp 65001 > nul");

    // 初始化GLFW
    if (!glfwInit()) {
        std::cout << "GLFW初始化失败" << std::endl;
        return -1;
    }
    generateHeightMap();
    std::cout << 111 <<std::endl;
    generateObstacles();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "简单3D游戏", NULL, NULL);
    if (window == NULL) {
        std::cout << "创建GLFW窗口失败" << std::endl;
        glfwTerminate();
        return -1;
    }


    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "初始化GLAD失败" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 创建着色器程序
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    unsigned int skyboxShader = createShaderProgram(skyboxVertexShaderSource, skyboxFragmentShaderSource);

    // 设置顶点数据
    unsigned int terrainVAO, terrainVBO, terrainEBO;
    setupTerrainVertices(terrainVAO, terrainVBO, terrainEBO);
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    setupSphereVertices(sphereVAO, sphereVBO, sphereEBO);
    unsigned int obstacleVAO, obstacleVBO;
    setupCubeVertices(obstacleVAO, obstacleVBO);

    unsigned int skyboxVAO, skyboxVBO;
    setupSkyboxVertices(skyboxVAO, skyboxVBO);

    // 加载纹理（如果文件不存在，创建默认纹理）
// 在main函数中加载纹理时，修改纹理参数设置
    unsigned int grassTexture;
    int width, height, nrChannels;
    unsigned char *data = stbi_load("assets/texture/wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &grassTexture);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        // 调整纹理参数，确保重复适当
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cout << "未找到草地纹理，使用默认绿色" << std::endl;
        grassTexture = createDefaultTexture(glm::vec3(0.2f, 0.6f, 0.2f)); // 绿色作为草地
    }

    unsigned int obstacleTexture;
    data = stbi_load("assets/texture/wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &obstacleTexture);
        glBindTexture(GL_TEXTURE_2D, obstacleTexture);

        // 确保障碍物纹理设置正确
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = GL_RGB;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cout << "未找到障碍物纹理，使用默认蓝色" << std::endl;
        obstacleTexture = createDefaultTexture(glm::vec3(0.0f, 0.0f, 1.0f)); // 蓝色作为障碍物
    }
    unsigned int goalTexture;
    data = stbi_load("assets/texture/wall.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &goalTexture);
        glBindTexture(GL_TEXTURE_2D, goalTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    } else {
        std::cout << "未找到目标纹理，使用默认红色" << std::endl;
        goalTexture = createDefaultTexture(glm::vec3(1.0f, 0.0f, 0.0f)); // 红色作为目标
    }

    unsigned int skyTexture = loadSkybox();

    // 设置着色器中的纹理单元
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    // 光源位置（太阳）
    glm::vec3 lightPos(500.0f, 1000.0f, 500.0f);
    goalPos.y = getHeight(goalPos.x, goalPos.z);



    // 渲染循环
    while (!glfwWindowShouldClose(window) && !reachedGoal) {
        // 计算帧时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processInput(window);

        // 渲染
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(75.0f), (float)SCR_WIDTH/SCR_HEIGHT, 0.1f, 1000.0f);


// 设置视图矩阵
        glm::mat4 view = glm::lookAt(
                cameraPos,
                cameraPos + cameraFront,
                cameraUp
        );

// 绘制草地平面
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

// 绘制地形
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "isSkybox"), 0);

        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(terrainVAO);
        glDrawElements(GL_TRIANGLES, (TERRAIN_SIZE * TERRAIN_SIZE * 6), GL_UNSIGNED_INT, 0);

// 绘制目标点
        glBindTexture(GL_TEXTURE_2D, goalTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, goalPos);
        glBindTexture(GL_TEXTURE_2D, goalTexture);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(goalPos.x, goalPos.y + 5.0f, goalPos.z)); // 在目标点上方
        model = glm::scale(model, glm::vec3(0.5f, 10.0f, 0.5f)); // 细长的柱子
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

// 为光柱设置半透明效果
        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 0.0f, 0.0f); // 红色
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 0); // 不使用纹理，直接用颜色

        glBindVertexArray(obstacleVAO); // 使用立方体VAO
        glDrawArrays(GL_TRIANGLES, 0, 36);

// 恢复设置
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
// 使目标点稍微旋转，增加视觉效果
        model = glm::rotate(model, (float) glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindVertexArray(sphereVAO);

        int indexCount;
        glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexCount);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        // 绘制障碍物
        glBindTexture(GL_TEXTURE_2D, obstacleTexture);
        for (const auto& obstacle : obstacles) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, obstacle.position);

            // 缩放调整
            model = glm::scale(model, glm::vec3(obstacle.radius * 2.0f));  // * 2 因为立方体顶点是从-0.5到0.5

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            // 在绘制前绑定纹理并设置正确的重复模式
            glBindTexture(GL_TEXTURE_2D, obstacleTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glBindVertexArray(obstacleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);  // 立方体有36个顶点
        }

// 绘制天空
// 调整视图矩阵以消除平移
        glm::mat4 skyView = glm::mat4(glm::mat3(view));

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(skyView));

        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(500.0f)); // 天空很大
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, skyTexture);
        glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), 1);
        glUniform1i(glGetUniformLocation(shaderProgram, "isSkybox"), 1);


        glDepthFunc(GL_LEQUAL);
        glBindVertexArray(skyboxVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);

// 交换缓冲并轮询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

// 清理资源
    glDeleteVertexArrays(1, &terrainVAO);
    glDeleteBuffers(1, &terrainVBO);
    glDeleteBuffers(1, &terrainEBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(skyboxShader);
    glDeleteVertexArrays(1, &obstacleVAO);
    glDeleteBuffers(1, &obstacleVBO);

    glfwTerminate();

    if (reachedGoal) {
        std::cout << "游戏顺利通关！" << std::endl;
    }

    return 0;
}
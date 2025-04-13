//
// Created by ROG on 2025/3/24.
// 统一OpenGL库的头文件的包含, 防止重复包含
//

#ifndef CORE_H
#define CORE_H

// glad的头文件引用必须放在glfw之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLM数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// GLM的字符串转换, 可用于将向量, 矩阵等类型转换为字符串(glm::to_string(xxx)), 但是使用前需要定义GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

#endif //CORE_H

//
// Created by ROG on 2025/3/24.
//

#include "shader.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const char* vsSrcPath, const char* fsSrcPath) {
    // vs, fs着色器的代码字符串
    std::string vsSrc, fsSrc;
    // 着色器的文件流(inFileStream)
    std::ifstream vsFile, fsFile;

    // 保证ifstream遇到问题时能抛出异常
    vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // 读取文件
    try {
        // 打开文件
        vsFile.open(vsSrcPath);
        fsFile.open(fsSrcPath);
        // 将文件输入流中的字符串输入到stringstream中
        std::stringstream vsStream, fsStream;
        vsStream << vsFile.rdbuf();
        fsStream << fsFile.rdbuf();
        vsFile.close();
        fsFile.close();
        // 将stringstream中的字符串转换为string
        vsSrc = vsStream.str();
        fsSrc = fsStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER: Shader File Error: " << e.what() << std::endl;
    }

    const char* vertexShaderSource = vsSrc.c_str();
    const char* fragmentShaderSource = fsSrc.c_str();

    // 1. 创建Shader程序
    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // 输入代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    // 编译Shader并检查编译结果
    glCompileShader(vertexShader);
    // 检查编译结果
    checkShaderError(vertexShader, "COMPILE");
    glCompileShader(fragmentShader);
    // 检查编译结果
    checkShaderError(fragmentShader, "COMPILE");

    // 2. 链接Shader程序
    // 创建Shader程序对象(全局变量)
    program = glCreateProgram();
    // 将编译好的vs和fs附加到Shader程序对象上
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    // 链接Shader程序, 形成一个完整的可执行Shader程序
    glLinkProgram(program);
    // 检查链接结果
    checkShaderError(program, "LINK");
    // 清理
    // 编译链接形成可执行Shader程序后, 着色器对象就不需要了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() = default;

void Shader::begin() const {
    glUseProgram(program);
}

void Shader::end() {
    glUseProgram(0);
}

void Shader::setBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), (int)value);
}
void Shader::setVec3(const std::string& name, const float v0, const float v1, const float v2) const {
    glUniform3f(glGetUniformLocation(program, name.c_str()), v0, v1, v2);
}
void Shader::setVec3(const std::string& name, const float* values) const {
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, values);
}
void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(program, name.c_str()), 1, &value[0]);
}
void Shader::setInt(const std::string& name, const int value) const {
    glUniform1i(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, const float value) const {
    glUniform1f(glGetUniformLocation(program, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    // count: 要传递的矩阵数量
    // transpose参数: 是否转置矩阵
    // 📌📌OpenGL和GLM的矩阵存储方式都是列主序, 所以不需要转置
    // 列主序: 列优先存储, 先存储列, 再存储行. 比如mat2((1, 2), (3, 4))会被存储为(1, 3, 2, 4)
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setMat3(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}


void Shader::checkShaderError(GLuint target, const std::string& type) {
    int success = 0;
    char infoLog[1024];
    if (type == "COMPILE") {
        glGetShaderiv(target, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(target, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
        }
    } else if (type == "LINK") {
        glGetProgramiv(target, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(target, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "ERROR::SHADER::" << type << "::LINKING_FAILED\n" << infoLog << std::endl;
        }
    } else {
        std::cerr << "ERROR::SHADER::" << type << "::UNKNOWN_ERROR\n";
    }
}
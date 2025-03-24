//
// Created by ROG on 2025/3/24.
//

#include "shader.h"
#include <error_check.h>

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
    // length为nullptr表示字符串以'\0'结尾('\0'结尾表示自然结尾, 无需传递长度)
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);

    // 编译Shader并检查编译结果
    int success = 0;
    char infoLog[1024];
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

Shader::~Shader() {}

void Shader::begin() {
    GL_CALL(glUseProgram(program));
}

void Shader::end() {
    GL_CALL(glUseProgram(0));
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
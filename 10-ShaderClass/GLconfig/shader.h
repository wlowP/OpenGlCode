//
// Created by ROG on 2025/3/24.
//

#ifndef SHADER_H
#define SHADER_H

#include "core.h"
#include <string>

/**
 * Shader封装为一个类
 * 文件读取 + 编译链接 + 错误检查
 */
class Shader {
public:
    Shader(const char* vsSrcPath, const char* fsSrcPath); // 构造函数
    ~Shader();

    void begin() const; // 开始使用当前的着色器

    static void end(); // 结束使用当前的着色器

    GLuint getProgram() const { return program; } // 获取当前的program

    // 设置uniform变量(注意着色器中得先有uniform定义)
    void setVec3(const std::string& name, float v0, float v1, float v2) const;
    void setVec3(const std::string& name, const float* values) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
private:
    // 对于shader程序, 检查编译错误; 对于program, 检查链接错误
    void checkShaderError(GLuint target, const std::string& type);
private:
    GLuint program{0};
};

#endif //SHADER_H
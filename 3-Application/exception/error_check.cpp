//
// Created by ROG on 2025/3/13.
//

#include "error_check.h"

#include <iostream>
#include <cassert>
#include <glad/glad.h>

// 检查OpenGL错误
void errorCheck() {
    // OpenGL不会因为参数错误直接崩溃, 仍可以不正常地运行(比如黑屏)
    // 此时用glGetError来检查错误, 其作用为获取最近发生的OpenGL错误的代码
    // 比如1281表示无效操作, 1282表示无效值, 1280表示无效枚举, 0表示没有错误
    // glClear(-1)可以导致1281错误
    const GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::string msg;
        switch (err) {
        case GL_INVALID_ENUM:
            msg = "Invalid enum";
            break;
        case GL_INVALID_VALUE:
            msg = "Invalid value";
            break;
        case GL_INVALID_OPERATION:
            msg = "Invalid operation";
            break;
        case GL_OUT_OF_MEMORY:
            msg = "Out of memory";
            break;
        default:
            msg = "Unknown error";
        }
        std::cerr << "OpenGL error: " << msg << std::endl;
        // 这里可以选择直接抛出异常, 或者直接assert(false)让程序停止运行
        assert(false);
    }
}
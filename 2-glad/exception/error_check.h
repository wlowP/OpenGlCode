//
// Created by ROG on 2025/3/13.
//

#ifndef ERROR_CHECK_H
#define ERROR_CHECK_H

// 在GL_CALL基础上再使用预编译宏来开关错误检查功能
// 定义(开关)DEBUG预编译宏的设置在CMakeLists.txt中
#ifdef DEBUG
// 定义一个宏来简化OpenGL函数调用和错误检查
// 省得每次调用gl函数之后都要再手动调用errorCheck
#define GL_CALL(function) \
function; \
errorCheck();
#else
#define GL_CALL(function) function
#endif

void errorCheck();

#endif //ERROR_CHECK_H
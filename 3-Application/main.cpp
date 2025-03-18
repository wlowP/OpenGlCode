#include <iostream>

// glad的头文件引用必须放在glfw之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 这里引用的是2-glad中的error_check.h, 在CMakeLists.txt中设置了include路径
#include "error_check.h"
#include "application/Application.h"

// 窗口尺寸变化的回调
void framebufferSizeCallback(const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
    // 使用glad中的glViewport来动态更新视口的大小
    glViewport(0, 0, width, height);
}

// 键盘输入的回调
/*
 * key: 字母按键码 scancode: 物理按键码(不常用) action: 按键动作(按下/抬起) mods: 修饰键(shift, ctrl, alt)
 */
void keyCallback(const int key, int scancode, const int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "key pressed: " << key << std::endl;
    }
}

/*
 * 1. 一个简单的GLFW窗口, 并且使用基本的glad函数
 * 2. 需要先使用glad加载所有当前版本所需的OpenGL函数指针(不同厂家显卡的OpenGL函数指针不同)
 *
 * 使用glGetError函数获取调用gl函数过程中出现的错误代码
 *
 * 将错误检测封装为宏, 并进一步封装为预编译宏以达到一键开关功能的效果
 *
 * 3. 封装单例的OpenGL应用类
 */
int main() {
    APP->test();
    if (!APP->init(800, 600, "封装成OpenGL Application类")) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置事件回调
    // 窗体尺寸变化
    APP->setOnResizeCallback(framebufferSizeCallback);
    // 键盘输入
    APP->setOnKeyboardCallback(keyCallback);

    // 设置OpenGL视口
    GL_CALL(glViewport(0, 0, 800, 600));
    // 设置擦除画面时的颜色. (擦除画面其实就是以另一种颜色覆盖当前画面)
    GL_CALL(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));

    // 3. 执行窗体循环. 📌📌每次循环为一帧
    // 窗体只要保持打开, 就会一直循环
    while (APP->update()) {
        // 执行画布清理操作(用glClearColor设置的颜色来清理(填充)画布)
        // 定义宏来封装errorCheck操作到函数调用之后
        // 设置glClear(-1)来触发错误检测
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT))

        // 渲染操作
    }

    // 4. 清理和关闭
    APP->destroy();
    return 0;
}
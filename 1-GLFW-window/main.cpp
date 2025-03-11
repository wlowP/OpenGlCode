#include <iostream>

// glad的头文件引用必须放在glfw之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
 * 一个简单的GLFW窗口
 */
int main() {
    // 1. 设置GLFW的初始环境
    if (!glfwInit()) {
        std::cerr << "GLFW初始化失败" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); // 次版本号. 因为glad是4.6的,所以这里也要设置为4.6
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 使用核心模式(非立即渲染模式)

    // 2. 创建窗体对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "一个GLFW窗口", NULL, NULL);
    if (!window) {
        std::cerr << "GLFW窗口创建失败" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 切换当前窗体为OpenGL绘制的当前区域
    glfwMakeContextCurrent(window);

    // 3. 执行窗体循环. 📌📌每次循环为一帧
    while (!glfwWindowShouldClose(window)) {

        // 接收并分发窗口消息(检查事件的消息队列)
        glfwPollEvents();
    }

    // 4. 清理和关闭
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
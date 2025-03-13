#include <iostream>

// glad的头文件引用必须放在glfw之前
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 窗口尺寸变化的回调
void framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
    // 窗体变化响应
    std::cout << "current window size: " << width << "x" << height << std::endl;
}

// 键盘输入的回调
/*
 * key: 字母按键码 scancode: 物理按键码(不常用) action: 按键动作(按下/抬起) mods: 修饰键(shift, ctrl, alt)
 */
void keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods) {
    if (action == GLFW_PRESS) {
        std::cout << "key pressed: " << key << std::endl;
    }
}

/*
 * 一个简单的GLFW窗口
 */
int main() {
    // 1. 设置GLFW的初始环境
    if (!glfwInit()) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return -1;
    }
    // 设置GLFW的版本号
    // 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // 次版本号. 因为glad是4.6的,所以这里也要设置为4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 使用核心模式(非立即渲染模式)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗体对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "一个GLFW窗口", nullptr, nullptr);
    if (!window) {
        std::cerr << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 切换当前窗体为OpenGL绘制的当前区域
    glfwMakeContextCurrent(window);

    // 设置事件回调
    // 窗体尺寸变化
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // 键盘输入
    glfwSetKeyCallback(window, keyCallback);

    // 初始化GLAD - 使用glad加载所有当前版本的OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "failed to initialize GLAD" << std::endl;
    }

    // 3. 执行窗体循环. 📌📌每次循环为一帧
    // 窗体只要保持打开, 就会一直循环
    while (!glfwWindowShouldClose(window)) {

        // 接收并分发窗口消息(检查事件的消息队列)
        glfwPollEvents();
    }

    // 4. 清理和关闭
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
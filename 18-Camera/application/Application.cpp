//
// Created by ROG on 2025/3/16.
//

#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// C++中初始化类的静态成员变量的语法. 必须在类外面进行初始化
Application* Application::instance = nullptr;

Application* Application::getInstance() {
    if (instance == nullptr) {
        instance = new Application();
    }
    return instance;
}

bool Application::init(const int& width, const int& height, const char* title) {
    // 1. 设置GLFW的初始环境
    if (!glfwInit()) {
        std::cerr << "failed to initialize GLFW" << std::endl;
        return false;
    }
    // 设置GLFW的版本号
    // 主版本号
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    // 次版本号. 因为glad是4.6的,所以这里也要设置为4.6
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 使用核心模式(非立即渲染模式)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. 创建窗体对象
    this->width = width;
    this->height = height;
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // 切换当前窗体为OpenGL绘制的当前区域
    glfwMakeContextCurrent(window);

    // 初始化GLAD - 使用glad加载所有当前版本的OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "failed to initialize GLAD" << std::endl;
        return false;
    }

    // 绑定glfw的事件回调
    // 窗体尺寸变化
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // 键盘输入
    glfwSetKeyCallback(window, keyCallback);
    // 鼠标点击
    glfwSetMouseButtonCallback(window, mouseCallback);
    // 鼠标移动
    glfwSetCursorPosCallback(window, cursorPosCallback);

    // 通过这个函数将app对象的指针附加到当前窗口, 以便在窗口相关的回调(键盘, 鼠标事件)中访问到app对象中各种自定义的数据
    glfwSetWindowUserPointer(window, this);

    glViewport(0, 0, width, height); // 设置OpenGL视口

    return true;
}

bool Application::update() {
    if (glfwWindowShouldClose(window)) {
        return false;
    }
    // 接收并分发窗口消息(检查事件的消息队列)
    glfwPollEvents();

    // 渲染操作...

    // 切换双缓存
    glfwSwapBuffers(window);
    return true;
}

void Application::destroy() {
    // 终止
    glfwTerminate();
}

/*
 * 窗体大小变化回调
 *  width/height: 当前窗体宽高
 */
void Application::framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->width = width;
    app->height = height;
    // 在这里调用用户实际设置的回调
    if (app->onResizeCallback == nullptr) {
        std::cout << "OnResizeCallback not provided" << std::endl;
        return;
    }
    app->onResizeCallback(width, height);
}

/*
 * 键盘输入回调
 * key: 键盘按键的ASCII码
 * scancode: 键盘按键的扫描码(不常用)
 * action: 按键状态: 按下, 松开
 * mods: 修饰符: shift, ctrl, alt
 */
void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onKeyboardCallback == nullptr) {
        std::cout << "OnKeyboardCallback not provided" << std::endl;
        return;
    }
    app->onKeyboardCallback(key, action, mods);
}

/*
 * 鼠标点击回调
 *  button: 按键类型: 左键, 右键, 中键
 *  action: 按键状态: 按下, 松开
 *  mods: 修饰符: shift, ctrl, alt
 */
void Application::mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onMouseCallback == nullptr) {
        std::cout << "OnMouseCallback not provided" << std::endl;
        return;
    }
    app->onMouseCallback(button, action, mods);
}

/*
 * 鼠标移动回调
 *  x, y: 当前鼠标坐标(屏幕像素位置, 而不是NDC坐标)
 */
void Application::cursorPosCallback(GLFWwindow* window, double x, double y) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onMouseMoveCallback == nullptr) {
        std::cout << "onMouseMoveCallback not provided" << std::endl;
        return;
    }
    app->onMouseMoveCallback(x, y);
}

void Application::getMousePosition(double& x, double& y) const {
    // 获取当前鼠标坐标(屏幕像素位置, 而不是NDC坐标)
    glfwGetCursorPos(window, &x, &y);
}


Application::Application() = default;

Application::~Application() = default;

//
// Created by ROG on 2025/3/29.
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
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "GLFW初始化失败" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 启用多重采样抗锯齿

    // 创建窗体对象
    this->width = width;
    this->height = height;
    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "创建窗体失败" << std::endl;
        glfwTerminate();
        return false;
    }

    // 切换当前窗体为OpenGL绘制的当前区域
    glfwMakeContextCurrent(window);

    // 初始化GLAD - 使用glad加载所有当前版本的OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "GLAD初始化失败" << std::endl;
        return false;
    }

    // 绑定glfw的事件回调
    // 窗体尺寸变化
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // 键盘输入
    glfwSetKeyCallback(window, keyCallback);
    // 鼠标输入
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    // 鼠标移动
    glfwSetCursorPosCallback(window, mouseMoveCallback);

    // 通过这个函数将app对象的指针附加到当前窗口, 以便在窗口相关的回调(键盘, 鼠标事件)中访问到app对象中各种自定义的数据
    glfwSetWindowUserPointer(window, this);

    glViewport(0, 0, width, height); // 设置OpenGL视口尺寸

    // 启用抗锯齿
    glEnable(GL_MULTISAMPLE);        // 启用多重采样
    glEnable(GL_LINE_SMOOTH);        // 启用线条抗锯齿
    glEnable(GL_POLYGON_SMOOTH);     // 启用多边形抗锯齿
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);  // 启用混合
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 设置线宽（对于线条抗锯齿）
    glLineWidth(1.5f);

    return true;
}

bool Application::update() {
    if (glfwWindowShouldClose(window)) {
        return false;
    }

    // 渲染操作...

    // 切换双缓存
    glfwSwapBuffers(window);
    // 接收并分发窗口消息(检查事件的消息队列)
    glfwPollEvents();
    return true;
}

void Application::close() {
    // 关闭窗口
    glfwSetWindowShouldClose(window, true);
}

void Application::destroy() {
    // 终止
    glfwTerminate();
}

void Application::framebufferSizeCallback(GLFWwindow* window, const int width, const int height) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    app->width = width;
    app->height = height;
    // 在这里调用用户实际设置的回调
    if (app->onResizeCallback == nullptr) {
        std::cout << "OnResizeCallback not provided" << std::endl;
        return;
    }
    app->onResizeCallback(window, width, height);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onKeyboardCallback == nullptr) {
        std::cout << "OnKeyboardCallback not provided" << std::endl;
        return;
    }
    app->onKeyboardCallback(window, key, scancode, action, mods);
}

void Application::mouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onMouseClickCallback == nullptr) {
        std::cout << "OnMouseClickCallback not provided" << std::endl;
        return;
    }
    app->onMouseClickCallback(window, button, action, mods);
}

void Application::mouseMoveCallback(GLFWwindow* window, double x, double y) {
    Application* app = (Application*)glfwGetWindowUserPointer(window);
    if (app->onMouseMoveCallback == nullptr) {
        std::cout << "OnMouseMoveCallback not provided" << std::endl;
        return;
    }
    app->onMouseMoveCallback(window, x, y);
}

Application::Application() = default;

Application::~Application() = default;

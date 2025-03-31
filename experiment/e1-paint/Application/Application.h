//
// Created by ROG on 2025/3/29.
//
#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstdint>
#include <iostream>

// 定义一个GLFWwindow类, 让C++编译器等到链接阶段再去找GLFW的库, 而不是编译阶段就找不到而报错
class GLFWwindow;

// 事件回调函数的函数指针类型
using OnResizeCallback = void(*)(GLFWwindow*, int, int);
// 键盘输入回调
using OnKeyboardCallback = void(*)(GLFWwindow*, int, int, int, int);
// 鼠标移动回调
using OnMouseMoveCallback = void(*)(GLFWwindow*, double, double);
// 鼠标点击回调
using OnMouseClickCallback = void(*)(GLFWwindow*, int, int, int);

/*
 * Application存储应用状态等数据
 */
class Application {
public:
    ~Application();
    static Application* getInstance();
    // 初始化GLFW. 返回值表示是否成功
    bool init(const int& width = 800, const int& height = 600, const char* title = "这是一个窗口");
    // 渲染, 更新视图. 返回值表示是否成功
    bool update();
    // 关闭窗口
    void close();
    // 销毁
    void destroy();

    // 获取窗口尺寸
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }

    // 用户设置事件回调
    // 窗体大小变化
    void setOnResizeCallback(const OnResizeCallback callback) {
        onResizeCallback = callback;
    }
    // 键盘输入
    void setOnKeyboardCallback(const OnKeyboardCallback callback) {
        onKeyboardCallback = callback;
    }
    // 鼠标移动
    void setOnMouseMoveCallback(const OnMouseMoveCallback callback) {
        onMouseMoveCallback = callback;
    }
    // 鼠标点击
    void setOnMouseClickCallback(const OnMouseClickCallback callback) {
        onMouseClickCallback = callback;
    }

    static void test() {
        std::cout << "application test" << std::endl;
    }
private:
    static Application* instance; // 全局唯一实例

    // uint32_t 等于 unsigned int
    // 窗口尺寸
    uint32_t width{0}; // 大括号初始化可以防止缩窄转换(隐式转换)
    uint32_t height{0};

    // 事件回调函数
    // 窗体尺寸变化的回调
    OnResizeCallback onResizeCallback{nullptr};
    // 键盘输入的回调
    OnKeyboardCallback onKeyboardCallback{nullptr};
    // 鼠标移动的回调
    OnMouseMoveCallback onMouseMoveCallback{nullptr};
    // 鼠标点击的回调
    OnMouseClickCallback onMouseClickCallback{nullptr};

    // 应用程序的窗口
    GLFWwindow* window{nullptr};

    // 实际执行的事件回调(绑定到GLFW的回调). 绑定的操作在init中进行
    // 设置为静态函数是为了方便引用(C++不允许指向成员函数的指针)
    // 窗体大小变化
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    // 键盘输入
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    // 鼠标移动
    static void mouseMoveCallback(GLFWwindow* window, double x, double y);
    // 鼠标点击
    static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);

    Application(); // 私有化构造方法
};

#endif //APPLICATION_H
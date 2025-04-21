//
// Created by ROG on 2025/3/16.
//
#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstdint>
#include <iostream>

// 定义一个GLFWwindow类, 让C++编译器等到链接阶段再去找GLFW的库, 而不是编译阶段就找不到而报错
class GLFWwindow;

// 事件回调函数的函数指针类型
using OnResizeCallback = void(*)(int width, int height);
// 键盘输入回调
using OnKeyboardCallback = void(*)(int key, int action, int modes);
// 鼠标点击回调
using OnMouseCallback = void(*)(int button, int action, int mods);
// 鼠标移动回调
using OnMouseMoveCallback = void(*)(double x, double y);
// 鼠标滚轮回调
using OnMouseScrollCallback = void(*)(double offsetX, double offsetY);

// 定义一个宏方便访问单例
#define APP Application::getInstance()

/*
 * Application代表应用程序本身, 是个单例类
 *
 * 主要成员函数: init(初始化), update(每一帧的渲染), destroy(结束)
 *
 * 绑定事件回调函数的步骤:
 *  1. 定义函数指针类型(OnResizeCallback)以及该类型的成员变量 以及该成员变量的setter
 *  2. 定义一个静态的函数用来实际执行回调(static void framebufferSizeCallback)
 *  3. 在init中使用glfw的函数绑定回调
 */
class Application {
public:
    ~Application();
    static Application* getInstance(); // 获取全局唯一实例

    // 初始化GLFW. 返回值表示是否成功
    bool init(const int& width = 800, const int& height = 600, const char* title = "OpenGL应用窗口");

    // 渲染, 更新视图. 返回值表示是否成功
    bool update();

    // 销毁
    void destroy();

    // 获取窗口尺寸
    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }

    // 获取鼠标位置(使用glfw的函数)
    void getMousePosition(double& x, double& y) const;

    // 用户设置事件回调(setter)
    // 窗体大小变化
    void setOnResizeCallback(const OnResizeCallback callback) {onResizeCallback = callback;}
    // 键盘输入
    void setOnKeyboardCallback(const OnKeyboardCallback callback) {onKeyboardCallback = callback;}
    // 鼠标点击输入
    void setOnMouseCallback(const OnMouseCallback callback) {onMouseCallback = callback;}
    // 鼠标移动输入
    void setOnMouseMoveCallback(const OnMouseMoveCallback callback) {onMouseMoveCallback = callback;}
    // 鼠标滚轮
    void setOnMouseScrollCallback(const OnMouseScrollCallback callback) {onMouseScrollCallback = callback;}

    static void test() {
        std::cout << "Application test" << std::endl;
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
    // 鼠标点击的回调
    OnMouseCallback onMouseCallback{nullptr};
    // 鼠标移动的回调
    OnMouseMoveCallback onMouseMoveCallback{nullptr};
    // 鼠标滚轮的回调
    OnMouseScrollCallback onMouseScrollCallback{nullptr};

    // 应用程序的窗口
    GLFWwindow* window{nullptr};

    // 实际执行的事件回调(绑定到GLFW的回调). 绑定的操作在init中进行
    // 设置为静态函数是为了方便引用(C++不允许指向成员函数的指针)
    // 窗体大小变化
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    // 键盘输入
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    // 鼠标点击输入
    static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
    // 鼠标移动输入
    static void cursorPosCallback(GLFWwindow* window, double x, double y);
    // 鼠标滚轮
    static void scrollCallback(GLFWwindow* window, double offsetX, double offsetY);

    Application(); // 私有化构造方法
};

#endif //APPLICATION_H
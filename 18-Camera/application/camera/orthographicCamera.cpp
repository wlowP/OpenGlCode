//
// Created by ROG on 2025/4/20.
//

#include "orthographicCamera.h"

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top, float near, float far)
    : left(left), right(right), bottom(bottom), top(top), near(near), far(far) {
}

OrthographicCamera::~OrthographicCamera() = default;

glm::mat4 OrthographicCamera::getProjectionMatrix() const {
    float scaleFactor = pow(2.0f, scale);
    return glm::ortho(left * scaleFactor, right * scaleFactor, bottom * scaleFactor, top * scaleFactor, near, far);
}

void OrthographicCamera::zoom(float deltaScale) {
    // 正交缩放, 需要改变正交投影盒的大小(只改变left, right, bottom, top, 不改变near, far因为可能导致被剪裁, 而且正交平行投影下缩放z轴没什么效果)
    // 📌📌也不能线性地改变投影盒尺寸, 因为会导致缩小比例减小到0, 甚至负数, 图像出现翻转的现象
    // 于是采用非线性的连续函数-->指数函数y=2^x
    // 成员变量记录一个累加值scale, 最终图像会缩放为原来的2^scale倍(在getProjectionMatrix中乘上系数)
    // 加负号更符合鼠标操作习惯(滚轮上滑放大, 下滑缩小)
    scale += -deltaScale;
}

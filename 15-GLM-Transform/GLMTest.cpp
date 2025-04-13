#include <iostream>

// GLM数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// GLM的字符串转换, 可用于将向量, 矩阵等类型转换为字符串(glm::to_string(xxx)), 但是使用前需要定义GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

int main() {
    system("chcp 65001 > nul");

    cout << "GLM数学库基本使用" << endl;

    // 向量
    vec2 v2(1.0f, 2.0f);
    vec2 v2_1(3.0f, 4.0f);
    vec3 v3(1.0f, 2.0f, 3.0f);
    vec3 v3_1(2.0f, 3.0f, 5.0f);
    vec4 v4(1.0f, 2.0f, 3.0f, 4.0f);
    cout << "v2: " << to_string(v2) << endl;
    // 基本运算
    cout << "v2 + v2_1 = " << to_string(v2 + v2_1) << endl;
    cout << "v2 - v2_1 = " << to_string(v2 - v2_1) << endl;
    // 📌glm重载的*运算符既不是点乘也不是叉乘, 而是分量相乘. /运算符也是分量相除
    cout << "v2 * v2_1 = " << to_string(v2 * v2_1) << endl;
    cout << "v2 / v2_1 = " << to_string(v2 / v2_1) << endl;
    // 点乘 dot
    cout << "v2 · v2_1 = " << to_string(dot(v2, v2_1)) << endl;
    // 叉乘 cross. 📌📌注意glm仅支持三维向量的叉乘
    cout << "v3 x v3_1 = " << to_string(cross(v3, v3_1)) << endl;

    // 矩阵
    // 矩阵初始化. glm自从0.9.9版本起, 默认将矩阵初始化为全0矩阵. 最好构造函数传入1.0f初始化为单位矩阵
    // 📌📌OpenGL和GLM的矩阵存储方式都是列主序, 所以不需要转置
    // 列主序: 列优先存储, 先存储列, 再存储行. 比如mat2((1, 2), (3, 4))会被存储为(1, 3, 2, 4)
    mat4 m4(1.0f);
    // 也可用内置模板函数获得单位矩阵
    mat4 m4_i = glm::identity<mat4>();

    mat3 m3(1.0f);
    cout << "m3: " << to_string(m3) << endl;
    mat3 m3_1(
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    );
    mat3 m3_2(
        1, 3, 5,
        2, 4, 6,
        9, 8, 7
    );
    // 基本运算
    cout << "m3 + m3_1 = " << to_string(m3 + m3_1) << endl;
    cout << "m3_1 - m3 = " << to_string(m3_1 - m3) << endl;
    cout << "m3_1 * m3_2 = " << to_string(m3_1 * m3_2) << endl;

    // 矩阵转置
    cout << "m3_1的转置: " << to_string(transpose(m3_1)) << endl;
    // 矩阵的逆
    cout << "m3的逆: " << to_string(inverse(m3)) << endl;
    // 矩阵的行列式
    cout << "m3的行列式: " << determinant(m3) << endl;
}

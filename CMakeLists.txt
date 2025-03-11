# 为方便多项目运行学习, 此cmake项目采用多目标编译的方式
# 运行多个main.cpp文件

# 需求的最低cmake版本
cmake_minimum_required(VERSION 3.30)

# 项目名称
project(OpenGlCode)

# 本工程支持的C++版本
set(CMAKE_CXX_STANDARD 20)

# =======================================================
# =======可执行的子项目====================================
# =======glfw, glad的库已在experiment/model中配置=========
# =======================================================
add_subdirectory(experiment/model)
add_subdirectory(1-GLFW-window)

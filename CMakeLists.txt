# 为方便多项目运行学习, 此cmake项目采用多目标编译的方式
# 运行多个main.cpp文件

# 需求的最低cmake版本
cmake_minimum_required(VERSION 3.30)

# 项目名称
project(OpenGlCode)

# 本工程支持的C++版本
set(CMAKE_CXX_STANDARD 20)

# glfw, glad的库文件
# 加载头文件. 其中的GLFW目录是glfw3的头文件目录, glad和KHR是glad的头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
# 加载源文件
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 设置静态链接, 以免编译时提示找不到libgcc等动态链接库
#set(CMAKE_EXE_LINKER_FLAGS -static)

# =======================================================
# =======可执行的子项目====================================
# experiment/model项目的glfw, glad库文件单独配置了, 用的是动态链接dll
# =======================================================
add_subdirectory(experiment/model)
add_subdirectory(1-GLFW-window)
add_subdirectory(2-glad)
add_subdirectory(3-Application)
add_subdirectory(4-VBO)
add_subdirectory(5-VAO)
add_subdirectory(6-Shader)
add_subdirectory(7-DrawArray)
add_subdirectory(8-EBO)
add_subdirectory(9-DrawColor)
add_subdirectory(10-ShaderClass)

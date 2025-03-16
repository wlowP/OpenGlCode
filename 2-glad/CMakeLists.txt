# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

add_subdirectory(exception)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(2-glad ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)

# 注意msvc编译glfw库不能在mingw环境下运行
target_link_libraries(2-glad ${PROJECT_SOURCE_DIR}/lib/libglfw3.a errorCheck)
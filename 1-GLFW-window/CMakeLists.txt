add_executable(1-GLFW-window main.cpp glad.c)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 注意msvc编译glfw库不能在mingw环境下运行
target_link_libraries(1-GLFW-window ${PROJECT_SOURCE_DIR}/lib/libglfw3.a)
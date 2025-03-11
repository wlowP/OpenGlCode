add_executable(1-GLFW-window main.cpp glad.c)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# 静态链接不知道为啥会出一大堆编译错误, 只能先动态链接
target_link_libraries(1-GLFW-window ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)


file(GLOB ASSETS ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/1-GLFW-window)
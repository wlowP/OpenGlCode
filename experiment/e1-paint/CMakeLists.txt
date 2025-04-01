add_executable(e1-paint ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

add_subdirectory(application)
add_subdirectory(ShaderConfig)

target_link_libraries(e1-paint ${PROJECT_SOURCE_DIR}/lib/libglfw3.a e1-application shaderConfig)

# 拷贝资源文件
file(GLOB ASSETS ./assets)
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/experiment/e1-paint)
# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# target_link_libraries直接链接了2-glad中的errorCheck库, 这里只需要包括2-glad中的对应头文件即可, 不需要再编译为库文件
# 仅当errorCheck库发生更新后, 才需要重新编译
#add_subdirectory(exception)
# 也直接链接了3-Application中的application库
#add_subdirectory(application)
#add_subdirectory(glConfig)
include_directories(
        ${PROJECT_SOURCE_DIR}/2-glad/exception
        ${PROJECT_SOURCE_DIR}/3-Application/application
        ${PROJECT_SOURCE_DIR}/10-ShaderClass/glConfig
)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(12-Texture ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)
add_executable(12-1-Repeat ${PROJECT_SOURCE_DIR}/glad/glad.c repeat.cpp)

# 注意msvc编译glfw库不能在mingw环境下运行
target_link_libraries(12-Texture ${PROJECT_SOURCE_DIR}/lib/libglfw3.a errorCheck application glConfig)
target_link_libraries(12-1-Repeat ${PROJECT_SOURCE_DIR}/lib/libglfw3.a errorCheck application glConfig)

# ======资源文件拷贝======
# 着色器源代码
file(GLOB ASSETS ./assets)
# 纹理贴图
file(GLOB ASSETS ${ASSETS} ${PROJECT_SOURCE_DIR}/assets)
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/12-Texture)
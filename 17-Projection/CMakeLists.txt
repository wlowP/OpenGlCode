# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# target_link_libraries直接链接了2-glad中的errorCheck库, 这里只需要包括2-glad中的对应头文件即可, 不需要再编译为库文件
# 仅当errorCheck库发生更新后, 才需要重新编译
#add_subdirectory(exception)
# 也直接链接了3-Application中的application库
#add_subdirectory(application)
#add_subdirectory(GLconfig)
include_directories(
        # 检查错误的宏定义
        ${PROJECT_SOURCE_DIR}/2-glad/exception
        # 应用程序头文件, 包含窗体对象, 尺寸等
        ${PROJECT_SOURCE_DIR}/3-Application/application
        # 着色器类头文件, 包含着色器对象, 着色器编译等
        ${PROJECT_SOURCE_DIR}/10-ShaderClass/GLconfig
        # 纹理类头文件, 包含纹理对象, 纹理加载等(MipMap)
        ${PROJECT_SOURCE_DIR}/14-MipMap/GLconfig
)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(17-Projection ${PROJECT_SOURCE_DIR}/glad/glad.c Orthographic.cpp)

target_link_libraries(17-Projection
        ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
        errorCheck
        application
        glConfig
        glConfig-texture-mipmap
)

# ======资源文件拷贝======
file(GLOB ASSETS
        # 着色器源代码
        ./assets
        # 纹理贴图
        ${PROJECT_SOURCE_DIR}/assets
)

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/17-Projection)
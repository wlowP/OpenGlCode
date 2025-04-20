# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# target_link_libraries直接链接了2-glad中的errorCheck库, 这里只需要包括2-glad中的对应头文件即可, 不需要再编译为库文件
# 仅当errorCheck库发生更新后, 才需要重新编译
#add_subdirectory(exception)
#add_subdirectory(GLconfig)
include_directories(
        # 检查错误的宏定义
        ${PROJECT_SOURCE_DIR}/2-glad/exception
        # 着色器类头文件, 包含着色器对象, 着色器编译等
        ${PROJECT_SOURCE_DIR}/10-ShaderClass/GLconfig
        # 纹理类头文件, 包含纹理对象, 纹理加载等(MipMap)
        ${PROJECT_SOURCE_DIR}/14-MipMap/GLconfig
)
# 应用程序头文件, 包含窗体对象, 尺寸等(适配相机系统), 以及相机系统类, 属于应用程序级别(因为其中并没有OpenGL的相关代码, 只是glm库的使用)
add_subdirectory(application)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(18-Camera ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)

target_link_libraries(18-Camera
        ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
        errorCheck
        application-with-camera
        glConfig
        glConfig-texture-mipmap
)

# ======资源文件拷贝======
file(GLOB ASSETS
        # 着色器源代码(加载了MVP矩阵变换)
        ${PROJECT_SOURCE_DIR}/17-Projection/assets
        # 纹理贴图
        ${PROJECT_SOURCE_DIR}/assets
)

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/18-Camera)
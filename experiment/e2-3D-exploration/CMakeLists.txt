# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# target_link_libraries直接链接了2-glad中的errorCheck库, 这里只需要包括2-glad中的对应头文件即可, 不需要再编译为库文件
# 仅当errorCheck库发生更新后, 才需要重新编译
#add_subdirectory(exception)
#add_subdirectory(GLconfig)
#add_subdirectory(application)
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
# 几何类头文件, 包含VBO, VAO, EBO等的创建和绑定(包围盒+包围球用于碰撞检测)
add_subdirectory(GLconfig)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(e2-3D-exploration ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)
add_executable(e2-3D-exploration-model ${PROJECT_SOURCE_DIR}/glad/glad.c model.cpp)

target_link_libraries(e2-3D-exploration
        ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
        errorCheck
        e2-application-with-camera
        glConfig
        glConfig-texture-mipmap
        e2-glConfig-geometry
)
target_link_libraries(e2-3D-exploration-model
        ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
)

# ======资源文件拷贝======
file(GLOB ASSETS
        # 着色器源代码 Uniform判断纯色模式或者纹理模式
        ./assets
        # 纹理贴图
        ${PROJECT_SOURCE_DIR}/assets
)

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/experiment/e2-3D-exploration)
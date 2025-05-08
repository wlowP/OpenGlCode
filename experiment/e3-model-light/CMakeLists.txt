# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# 应用程序头文件, 包含窗体对象, 尺寸等(适配相机系统), 以及相机系统类, 属于应用程序级别(因为其中并没有OpenGL的相关代码, 只是glm库的使用)
add_subdirectory(application)
# 几何类, 封装了VBO, VAO, EBO等的创建和绑定; 纹理类头文件, 包含纹理对象, 纹理加载等(MipMap); 着色器类头文件, 包含着色器对象, 着色器编译等
add_subdirectory(GLconfig)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(e3-model-light ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)
target_link_libraries(e3-model-light
        ${PROJECT_SOURCE_DIR}/lib/libglfw3.a
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll
        e3-application-with-camera
        e3-glConfig
)

# ======资源文件拷贝======
file(GLOB ASSETS
        # 着色器源代码(加载了MVP矩阵变换, blinn-phong光照)
        ./assets
        # 纹理贴图
        ${PROJECT_SOURCE_DIR}/assets
        # assimp的动态库
        ${PROJECT_SOURCE_DIR}/lib/libassimp-5.dll
)

file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/experiment/e3-model-light)
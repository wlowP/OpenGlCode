# 本工程主程序文件以及输出程序名称
# OpenGlCode为输出的exe程序的名称, main.cpp和src/glad.c为构建这个可执行文件所需的源文件
# ==> 将main.cpp和src/glad.c编译成可执行的OpenGlCode.exe
# 其中glad.c是glad的源文件, 实则为glad的依赖库
add_executable(OpenGlCode src/glad.c main.cpp)

# 加载头文件
# 其中的GLFW目录是glfw3的头文件目录, glad和KHR是glad的头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
# 加载源文件
link_directories(${PROJECT_SOURCE_DIR}/lib)
# 将lib目录下的glfw3.dll文件链接到OpenGlCode可执行文件
target_link_libraries(OpenGlCode ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)

# ======资源文件拷贝======
# 获取./lib/glfw3.dll作为文件列表, 名为ASSETS
file(GLOB ASSETS ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
# 将ASSETS中的文件拷贝到输出目录
file(COPY ${ASSETS} DESTINATION ${CMAKE_BINARY_DIR}/experiment/model)

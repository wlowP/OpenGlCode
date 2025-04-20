# 递归将本目录下所有cpp源文件搜索并组成名为v的变量
# 会递归搜索
file(GLOB_RECURSE applicationSrc ./*.cpp)

# 将applicationSrc变量中的源文件编译成名为application-with-camera的库文件
add_library(application-with-camera ${applicationSrc})

target_link_libraries(application-with-camera ${PROJECT_SOURCE_DIR}/lib/libglfw3.a)
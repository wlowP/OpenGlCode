# 递归将本目录下所有cpp源文件搜索并组成名为glConfigSrc的变量
# 会递归搜索. CONFIGURE_DEPENDS会让CMake在每次编译时都检查文件是否有更新, 防止源文件发生变化时没有被重新编译
file(GLOB_RECURSE glConfigSrc CONFIGURE_DEPENDS ./*.cpp)

# 将glConfigSrc变量中的源文件编译成名为glConfig的库文件
add_library(glConfig ${glConfigSrc})

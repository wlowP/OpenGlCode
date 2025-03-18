# 递归将本目录下所有cpp源文件搜索并组成名为errorCheckSrc的变量
# 会递归搜索
file(GLOB_RECURSE errorCheckSrc ./*.cpp)

# 将errorCheckSrc变量中的源文件编译成名为errorCheck的库文件
add_library(errorCheck ${errorCheckSrc})
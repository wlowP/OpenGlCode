# 递归将本目录下所有cpp源文件搜索并组成名为MY_LIB_SOURCES的变量
# 会递归搜索
file(GLOB_RECURSE errorCheckSrc ./*.cpp)

# 将MY_LIB_SOURCES变量中的源文件编译成名为my_lib的库文件
add_library(errorCheck1 ${errorCheckSrc})
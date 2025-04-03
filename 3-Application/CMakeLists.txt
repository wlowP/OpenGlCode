# 开启预编译宏
# 格式: -D宏名称
add_definitions(-DDEBUG)

# target_link_libraries直接链接了2-glad中的errorCheck库, 这里只需要包括2-glad中的对应头文件即可, 不需要再编译为库文件
# 仅当errorCheck库发生更新后, 才需要重新编译
# add_subdirectory(exception)
include_directories(${PROJECT_SOURCE_DIR}/2-glad/exception)
add_subdirectory(application)

# glad的库是个源代码文件, 所以得编译进可执行文件
add_executable(3-Application ${PROJECT_SOURCE_DIR}/glad/glad.c main.cpp)


target_link_libraries(3-Application ${PROJECT_SOURCE_DIR}/lib/libglfw3.a errorCheck application)

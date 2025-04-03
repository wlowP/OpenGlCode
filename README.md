## OpenGLCode

- C++语言下的OpenGL学习代码仓库

### 抽离为单独运行项目

- 本项目用CMake构建, 并且为了达到阶段性增量学习的效果, 将多个可执行项目安排到各自的目录中用各自的CMakeLists.txt组织起来.

- 如果要将某一个子项目单独抽取出来, 注意要先将CMakeLists.txt中引用其他子项目的资源全部拷贝到该子项目(包括头文件, 资源文件)

- 有多个子项目都包含GLconfig目录. 抽取时直接合并所有GLconfig目录的文件, 并且仅保留其中一个目录的CMakeLists.txt即可. (注意其中add_library的库的名称要跟上级CMakeLists.txt统一)
  - 然后上级CMakeLists.txt中include_directories, add_executable中还保持着原来拆分的各个glConfig库名称, 仅保留一个(与上文所述的统一)即可

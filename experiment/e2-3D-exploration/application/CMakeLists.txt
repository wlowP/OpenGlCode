file(GLOB_RECURSE applicationSrc ./*.cpp)

add_library(e2-application-with-camera ${applicationSrc})

target_link_libraries(e2-application-with-camera ${PROJECT_SOURCE_DIR}/lib/libglfw3.a e2-glConfig-geometry)
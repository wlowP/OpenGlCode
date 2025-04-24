file(GLOB_RECURSE glConfigSrc CONFIGURE_DEPENDS ./*.cpp)

add_library(e2-glConfig-geometry ${glConfigSrc})

target_link_libraries(e2-glConfig-geometry glConfig-texture-mipmap)
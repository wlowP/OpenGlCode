file(GLOB_RECURSE glConfigSrc CONFIGURE_DEPENDS ./*.cpp)

add_library(glConfig-geometry ${glConfigSrc})

target_link_libraries(glConfig-geometry glConfig-texture-mipmap)
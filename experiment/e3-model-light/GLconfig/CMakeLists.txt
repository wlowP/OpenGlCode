file(GLOB_RECURSE glConfigSrc CONFIGURE_DEPENDS ./*.cpp)

add_library(e3-glConfig ${glConfigSrc})

target_link_libraries(e3-glConfig)
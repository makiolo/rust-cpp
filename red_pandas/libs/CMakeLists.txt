# Third-Party Core
set(ta-lib_SOURCE_DIR ${CMAKE_SOURCE_DIR}/depends/ta-lib-main)
add_subdirectory(depends/ta-lib-main)

set(ENTITYX_MAX_COMPONENTS 128 CACHE STRING "Set the maximum number of components.")
set(ENTITYX_DT_TYPE std::shared_ptr<Serie> CACHE STRING "The type used for delta time in EntityX update methods.")
set(ENTITYX_BUILD_SHARED false CACHE BOOL "Build shared libraries?")
set(ENTITYX_BUILD_TESTING false CACHE BOOL "Enable building of tests.")
add_subdirectory(depends/entityx-12-2-2023)

add_subdirectory(depends/mdspan-stable)
# Move to core-math lib
# add_subdirectory(depends/eigen-3.4.0)
# add_subdirectory(depends/libinterpolate)
# add_subdirectory(depends/libintegrate)

# Excel bridge
add_subdirectory(depends/excelbind/src)

# internal Core
enable_testing()
include(libs/core/CMakeLists.txt)
include(libs/finance/CMakeLists.txt)
include(libs/portfolio/CMakeLists.txt)
include(libs/simulator/CMakeLists.txt)

# optional GUI
# add_subdirectory(depends/implot_demos)

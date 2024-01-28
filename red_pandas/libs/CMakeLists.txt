# Foundation Layer
# talib + entityx + pytorch||eigen + libinterpolate + libintegrate
#
set(ta-lib_SOURCE_DIR ${CMAKE_SOURCE_DIR}/depends/ta-lib-main)
add_subdirectory(depends/ta-lib-main)

set(ENTITYX_MAX_COMPONENTS 128 CACHE STRING "Set the maximum number of components.")
set(ENTITYX_DT_TYPE std::shared_ptr<Serie> CACHE STRING "The type used for delta time in EntityX update methods.")
set(ENTITYX_BUILD_SHARED false CACHE BOOL "Build shared libraries?")
set(ENTITYX_BUILD_TESTING false CACHE BOOL "Enable building of tests.")
add_subdirectory(depends/entityx-12-2-2023)
add_subdirectory(depends/mdspan-stable)

# Add PyTorch or eigen like backend to calculate
# add_subdirectory(depends/eigen-3.4.0)
# add_subdirectory(depends/libinterpolate)
# add_subdirectory(depends/libintegrate)
# add_subdirectory(depends/mimalloc-master)
# non-linear solvers

# Core Layer
enable_testing()
include(libs/core/CMakeLists.txt)
include(libs/finance/CMakeLists.txt)
include(libs/portfolio/CMakeLists.txt)
include(libs/simulator/CMakeLists.txt)


# Excel bridge (use core for direct Access from Excel to C++) (use pybind for call Python Scripts User)
# add_subdirectory(depends/excelbind/src)
# install(TARGETS excelbind DESTINATION ${PYTHON_INSTALL})
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/depends/excelbind/examples/excelbind.conf DESTINATION ${PYTHON_INSTALL})
# target_include_directories(excelbind PUBLIC depends/pybind11/include)

# optional GUI (use core for calculate prices, greeks) (use pybind for call Python Scripts User)
# add_subdirectory(depends/implot_demos)
# target_include_directories(stocks PUBLIC depends/pybind11/include)

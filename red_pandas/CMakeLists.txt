cmake_minimum_required(VERSION 3.23.2)
cmake_policy(SET CMP0116 OLD)

# config it
# project(red_pandas_project LANGUAGES CXX)
project(red_pandas_project)

# need mkl-include in conda
# conda install -c intel mkl mkl-devel
# conda install -c anaconda boost
# https://www.intel.com/content/www/us/en/docs/oneapi/installation-guide-windows/2024-0/conda.html

set(PROJECT_VERSION "1.0.0")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

if(NOT DEFINED ENV{PYTHON_HOME})
    message(FATAL_ERROR "not defined environment variable PYTHON_HOME. Example: \"C:\\Miniconda2\"")
endif()

if(NOT DEFINED ENV{PYTHON_LIB})
    message(FATAL_ERROR "not defined environment variable PYTHON_LIB. Example: \"python27\"")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
message("-- detected mode ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /MDd")
    add_link_options("/DEBUG")
    add_compile_definitions("_DISABLE_VECTOR_ANNOTATION")
    # add_compile_definitions("-DMIMALLOC_SHOW_STATS=1")
    message("-- add debug options")
else()
    # dynamic linkage to C runtime for let malloc/free replace
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
    message("-- add release options")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /std:c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++20")

IF(NOT MSVC)
    message("-- using gcc/clang flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")
else()
    message("-- using msvc flags")
    add_compile_definitions("_LIBCPP_DISABLE_DEPRECATION_WARNINGS=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /EHs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHs")
    # add_compile_definitions("_CRT_NONSTDC_NO_DEPRECATE")
    # add_compile_definitions("_CRT_SECURE_NO_WARNINGS")
endif()

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message("-- detected platform win64")
else()
    message("-- detected platform win32")
endif()
set(PYTHONHOME "$ENV{PYTHON_HOME}")
set(PYTHONLIBNAME "$ENV{PYTHON_LIB}.lib")
message("Using Python home: ${PYTHONHOME}. Linkage with: ${PYTHONLIBNAME}")

set(PYTHON3_LIB "${PYTHONHOME}/libs/${PYTHONLIBNAME}")
set(PYTHON3_INCLUDE "${PYTHONHOME}/include")
set(PYTHON3_EXE "${PYTHONHOME}/python.exe")
set(MKL_LIB
    mkl_intel_lp64_dll.lib
    mkl_intel_thread_dll.lib
    mkl_core_dll.lib
)
set(NUMPY_INCLUDE "${PYTHONHOME}/Lib/site-packages/numpy/core/include")
set(NUMPY_LIB "${PYTHONHOME}/Lib/site-packages/numpy/core/lib/npymath.lib")
set(MKL_INCLUDE "${PYTHONHOME}/Library/include")
set(MKL_LIBDIR "${PYTHONHOME}/Library/lib")
set(THIRD_LIB "${PYTHONHOME}/Library/bin")
set(PYTHON_BASEDIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python")
set(PYTHON_INSTALL "${PYTHONHOME}/Lib/site-packages/red_pandas")
set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dist")

include(libs/CMakeLists.txt)

# install all targets
get_property(target_names DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY BUILDSYSTEM_TARGETS)
install(TARGETS ${target_names} DESTINATION ${CMAKE_INSTALL_PREFIX})

# cpack

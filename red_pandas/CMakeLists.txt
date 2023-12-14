cmake_minimum_required(VERSION 3.23.2)
cmake_policy(SET CMP0116 OLD)

# config it
project(red_pandas_project LANGUAGES CXX)
# project(red_pandas_project)

# need mkl-include in conda
# conda install -c intel mkl mkl-devel
# conda install -c anaconda boost
# https://www.intel.com/content/www/us/en/docs/oneapi/installation-guide-windows/2024-0/conda.html

set(PROJECT_VERSION "1.0.0")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("-- detected mode ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Od /Zi /MDd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /MDd")
    add_link_options("/DEBUG")
    # add_compile_definitions("-DMIMALLOC_SHOW_STATS=1")
    message("-- add debug options")
else()
    # dynamic linkage to C runtime for let malloc/free replace
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MD")
    message("-- add release options")
endif()

IF(NOT MSVC)
    message("-- using gcc/clang flags")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")
else()
    message("-- using msvc flags")
    add_compile_definitions("-D_LIBCPP_DISABLE_DEPRECATION_WARNINGS=1")
endif()

# TODO: need remove it
set(EXTERNALLIBS "")

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message("-- detected platform win64")
    # set(PYTHONHOME "C:/Miniconda2")
    # set(PYTHONLIBNAME "python27.lib")
    set(PYTHONHOME "D:/miniconda311")
    set(PYTHONLIBNAME "python311.lib")
else()
    message("-- detected platform win32")
    set(PYTHONHOME "C:/Python27")
    set(PYTHONLIBNAME "python27.lib")
    # set(PYTHONHOME "C:/Python310")
    # set(PYTHONLIBNAME "python310.lib")
endif()
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
# set(THIRD_LIB "${CMAKE_CURRENT_SOURCE_DIR}/depends/libtorch/lib")
# set(THIRD_LIB "C:\\dev\\rust\\rust-cpp\\red_pandas\\depends\\libtorch\\lib")

include(libs/CMakeLists.txt)

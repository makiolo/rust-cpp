cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0116 OLD)

# config it
# project(red_pandas_project LANGUAGES CXX)
project(red_pandas_project)

set(PROJECT_VERSION "1.0.0")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations -Wno-error=register")

add_compile_definitions("-D_LIBCPP_DISABLE_DEPRECATION_WARNINGS=1")

# TODO: need remove it
set(EXTERNALLIBS C:/dev/ExternalLibs)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(PYTHONHOME "C:/Miniconda2")
    set(PYTHONLIBNAME "python27.lib")
    # set(PYTHONHOME "C:/Miniconda3")
    # set(PYTHONLIBNAME "python310.lib")
else()
    set(PYTHONHOME "C:/Python27")
    set(PYTHONLIBNAME "python27.lib")
    # set(PYTHONHOME "C:/Python310")
    # set(PYTHONLIBNAME "python310.lib")
endif()
set(PYTHON3_LIB "${PYTHONHOME}/libs/${PYTHONLIBNAME}")
set(PYTHON3_INCLUDE "${PYTHONHOME}/include")
set(PYTHON3_EXE "${PYTHONHOME}/python.exe")

# need mkl-include in conda
# conda install -c anaconda mkl-include

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message("-- detected win64")
    set(MKL_INCLUDE "${EXTERNALLIBS}/MKL/MKL_2019/include")
    set(MKL_LIBDIR "${EXTERNALLIBS}/MKL/MKL_2019/lib/intel64")
    set(MKL_LIB
        mkl_intel_lp64_dll.lib
        mkl_intel_thread_dll.lib
        mkl_core_dll.lib
    )
    set(NUMPY_INCLUDE "${PYTHONHOME}/Lib/site-packages/numpy/core/include")
    set(NUMPY_LIB "${PYTHONHOME}/Lib/site-packages/numpy/core/lib/npymath.lib")
    set(THIRD_LIB "${EXTERNALLIBS}/dll/x64")
else()
    message("-- detected win32")
    set(NUMPY_INCLUDE "${PYTHONHOME}/Lib/site-packages/numpy/core/include")
    set(NUMPY_LIB "${PYTHONHOME}/Lib/site-packages/numpy/core/lib/npymath.lib")
    set(MKL_INCLUDE "${EXTERNALLIBS}/MKL/MKL_2019/include")
    set(MKL_LIBDIR "${EXTERNALLIBS}/MKL/MKL_2019/lib/ia32")
    set(MKL_LIB
        mkl_intel_c_dll.lib
        mkl_intel_thread_dll.lib
        mkl_core_dll.lib
    )
    set(THIRD_LIB "${EXTERNALLIBS}/dll/win32")
endif()

include(libs/CMakeLists.txt)
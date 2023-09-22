cmake_minimum_required(VERSION 3.25)
cmake_policy(SET CMP0116 OLD)

# config it
project(red_pandas_project LANGUAGES CXX)
set(PROJECT_VERSION "1.0.0")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(EXTERNALLIBS C:/dev/meta3/ExternalLibs)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    message("-- detected win64 (using python 2)")
    set(PYTHONHOME "C:/Miniconda2")
    set(PYTHON3_LIB "${PYTHONHOME}/libs/python27.lib")
#    message("-- detected win64 (using python 3)")
#    set(PYTHONHOME "C:/Miniconda3")
#    set(PYTHON3_LIB "${PYTHONHOME}/libs/python310.lib")
    set(PYTHON3_INCLUDE "${PYTHONHOME}/include")
    set(PYTHON3_EXE "${PYTHONHOME}/python.exe")
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
    message("-- detected win32 (using python 2)")
    set(PYTHONHOME "C:/Python27")
    set(PYTHON3_LIB "${PYTHONHOME}/libs/python27.lib")
#    message("-- detected win32 (using python 3)")
#    set(PYTHONHOME "C:/Python310")
#    set(PYTHON3_LIB "${PYTHONHOME}/libs/python310.lib")
    set(PYTHON3_INCLUDE "${PYTHONHOME}/include")
    set(PYTHON3_EXE "${PYTHONHOME}/python.exe")
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

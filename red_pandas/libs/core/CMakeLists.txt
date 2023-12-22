
set(PRIVATE_sources
        libs/core/src/div.cpp
        libs/core/src/mul.cpp
        libs/core/src/sub.cpp
        libs/core/src/sum.cpp
        libs/core/src/exp.cpp
        libs/core/src/log.cpp
        libs/core/src/pow.cpp
        libs/core/src/sqrt.cpp
        libs/core/src/erf.cpp
        libs/core/src/neg.cpp
        libs/core/src/abs.cpp
        libs/core/src/operators/less.cpp
        libs/core/src/operators/isfinite.cpp
        libs/core/src/pdf.cpp
        libs/core/src/cdf.cpp
        libs/core/src/ppf.cpp
        libs/core/src/max0.cpp
        libs/core/src/min0.cpp
        libs/core/src/rsi.cpp
        libs/core/src/range.cpp
        libs/core/src/shift.cpp
        libs/core/src/diff.cpp
        libs/core/src/clean.cpp
        libs/core/src/rand_normal.cpp
        libs/core/src/categorical.cpp
        libs/core/src/serie.cpp
        libs/core/src/serie2d.cpp
        libs/core/src/calculation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/interface_py_wrap.cxx
        libs/core/src/formulas/greeks.cpp
        libs/core/src/formulas/volatility.cpp
        )
set(PUBLIC_sources
        libs/core/include/serie.h
        libs/core/include/serie2d.h
        libs/core/include/calculation.h
        libs/core/include/common.h
        libs/core/include/memoize/memoize.hpp
        libs/core/include/div.h
        libs/core/include/mul.h
        libs/core/include/sub.h
        libs/core/include/sum.h
        libs/core/include/exp.h
        libs/core/include/log.h
        libs/core/include/pow.h
        libs/core/include/sqrt.h
        libs/core/include/erf.h
        libs/core/include/neg.h
        libs/core/include/abs.h
        libs/core/include/operators/less.h
        libs/core/include/pdf.h
        libs/core/include/cdf.h
        libs/core/include/ppf.h
        libs/core/include/max0.h
        libs/core/include/min0.h
        libs/core/include/rsi.h
        libs/core/include/range.h
        libs/core/include/shift.h
        libs/core/include/diff.h
        libs/core/include/rand_normal.h
        libs/core/include/categorical.h
        libs/core/include/util.h
        libs/core/include/api.h
        libs/core/include/IterableRepository.h
        libs/core/include/formulas/greeks.h
        libs/core/include/formulas/volatility.h
        )
set(PUBLIC_extra_sources
        libs/core/tests/test_core.py
        libs/core/python/red_pandas.py
        libs/core/python/red_pandas_impl.py
        libs/core/python/interface_py.i
        libs/core/python/numpy.i
        libs/core/python/swig_generate.cmd
        )
set(PUBLIC_include_dirs
        libs/core/include
        depends/NumCpp-master/include
        depends/parallel-hashmap
        # depends/libtorch/include
        # depends/libtorch/include/torch/csrc/api/include
        ${PYTHON3_INCLUDE}
        ${MKL_INCLUDE}
        ${NUMPY_INCLUDE}
        )
set(PUBLIC_link_dirs
        ${MKL_LIBDIR}
        # depends/libtorch/lib
        )
set(PUBLIC_options
        )
set(PUBLIC_definitions
        NUMCPP_NO_USE_BOOST=1
        NUMCPP_USE_MULTITHREAD=0
        SWIG_PYTHON_INTERPRETER_NO_DEBUG=1
        )
set(PRIVATE_targets
        ${PYTHON3_LIB}
        ${NUMPY_LIB}
        ta_lib_static
        # torch_cpu
        # c10
        )

set(PUBLIC_targets
        std::mdspan
        npv_static
        simulator
        )

# add AUTOGEN code
file(GLOB_RECURSE public_sources libs/core/include/*.h)
add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/interface_py_wrap.cxx
        COMMAND swig_generate.cmd
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/interface_py.i
        DEPENDS ${public_sources} ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/red_pandas_impl.py ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/javascript/interface_py.i
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python
)

# create SHARED library
add_library(red_pandas SHARED)
target_sources(red_pandas PRIVATE ${PRIVATE_sources} PUBLIC ${PUBLIC_sources} ${PUBLIC_extra_sources})
target_include_directories(red_pandas PUBLIC ${PUBLIC_include_dirs})
target_compile_options(red_pandas PUBLIC ${PUBLIC_options})
target_compile_features(red_pandas PUBLIC cxx_std_20)
target_compile_definitions(red_pandas PUBLIC ${PUBLIC_definitions} RELEASE_PYTHON_THREAD=1 MKL_ALLOCATOR=0)
set_target_properties(red_pandas PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(red_pandas PROPERTIES PREFIX "_")
set_target_properties(red_pandas PROPERTIES SUFFIX ".pyd")
target_link_directories(red_pandas PUBLIC ${PUBLIC_link_dirs})
target_link_libraries(red_pandas PRIVATE ${PRIVATE_targets})
target_link_libraries(red_pandas PUBLIC ${PUBLIC_targets})


# create STATIC library
add_library(red_pandas_static STATIC)
# target_sources(red_pandas_static PRIVATE ${PRIVATE_sources} depends/mimalloc-master/src/static.c libs/core/src/allocator.cpp PUBLIC ${PUBLIC_sources} ${PUBLIC_extra_sources})
# target_include_directories(red_pandas_static PUBLIC ${PUBLIC_include_dirs} depends/mimalloc-master/include)
target_sources(red_pandas_static PRIVATE ${PRIVATE_sources} libs/core/src/allocator.cpp PUBLIC ${PUBLIC_sources} ${PUBLIC_extra_sources})
target_include_directories(red_pandas_static PUBLIC ${PUBLIC_include_dirs})
target_compile_options(red_pandas_static PUBLIC ${PUBLIC_options})
target_compile_features(red_pandas_static PUBLIC cxx_std_20)
target_compile_definitions(red_pandas_static PUBLIC ${PUBLIC_definitions} RELEASE_PYTHON_THREAD=0 MKL_ALLOCATOR=1)
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     target_compile_options(red_pandas_static PUBLIC "/fsanitize=address")
# endif()
# target_compile_definitions(red_pandas_static PUBLIC ${PUBLIC_definitions} RELEASE_PYTHON_THREAD=0 MKL_ALLOCATOR=0)
set_target_properties(red_pandas_static PROPERTIES VERSION ${PROJECT_VERSION})
target_link_directories(red_pandas_static PUBLIC ${PUBLIC_link_dirs})
target_link_libraries(red_pandas_static PRIVATE ${PRIVATE_targets} ${MKL_LIB})
# target_link_libraries(red_pandas_static PRIVATE ${PRIVATE_targets})
target_link_libraries(red_pandas_static PUBLIC ${PUBLIC_targets})

# tests LIBRARY
add_executable(test_core)
set(PRIVATE_tests_sources
        depends/Catch2/src/catch_amalgamated.cpp
        libs/core/tests/test_core.cpp
        )
set(PUBLIC_tests_sources
        depends/Catch2/include/catch_amalgamated.hpp
        )
set(PUBLIC_include_dirs
        depends/Catch2/include
        )
set(PUBLIC_targets
        red_pandas_static
        )
target_sources(test_core PRIVATE ${PRIVATE_tests_sources} PUBLIC ${PUBLIC_tests_sources})
target_include_directories(test_core PUBLIC ${PUBLIC_include_dirs})
target_compile_features(test_core PUBLIC cxx_std_20)
target_link_libraries(test_core PUBLIC ${PUBLIC_targets})

# test c++
add_test(	NAME test_core_cpp
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test_core.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests)
set_tests_properties(test_core_cpp PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;$ENV{PATH}")

# test python
add_test(	NAME test_core_py
        COMMAND ${PYTHON3_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests/test_core.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests)
set_tests_properties(test_core_py PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}\;${PYTHONHOME}\;${PYTHONHOME}/Scripts\;${PYTHONHOME}/Library/bin\;$ENV{PATH};PYTHONHOME=${PYTHONHOME};PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")



add_executable(test_volatility)
set(PRIVATE_tests_sources
        depends/Catch2/src/catch_amalgamated.cpp
        libs/core/tests/test_volatility.cpp
        )
set(PUBLIC_tests_sources
        depends/Catch2/include/catch_amalgamated.hpp
        )
set(PUBLIC_include_dirs
        depends/Catch2/include
        )
set(PUBLIC_targets
        red_pandas_static
        )
target_sources(test_volatility PRIVATE ${PRIVATE_tests_sources} PUBLIC ${PUBLIC_tests_sources})
target_include_directories(test_volatility PUBLIC ${PUBLIC_include_dirs})
target_compile_features(test_volatility PUBLIC cxx_std_20)
target_link_libraries(test_volatility PUBLIC ${PUBLIC_targets})

# test c++
add_test(	NAME test_volatility_cpp
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test_volatility.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests)
set_tests_properties(test_volatility_cpp PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;$ENV{PATH}")

# test python
add_test(	NAME test_volatility_py
        COMMAND ${PYTHON3_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests/test_volatility.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/tests)
set_tests_properties(test_volatility_py PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}\;${PYTHONHOME}\;${PYTHONHOME}/Scripts\;${PYTHONHOME}/Library/bin\;$ENV{PATH};PYTHONHOME=${PYTHONHOME};PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")


# copy resources to dist
install(TARGETS red_pandas DESTINATION ${PYTHON_INSTALL})
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python/red_pandas.py DESTINATION ${PYTHON_INSTALL} RENAME __init__.py)

# test stocks
add_test(
        NAME test_stocks
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/depends/implot_demos/${CMAKE_BUILD_TYPE}/stocks.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/depends/implot_demos/${CMAKE_BUILD_TYPE})
set_tests_properties(test_stocks PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;${CMAKE_CURRENT_BINARY_DIR}/depends/implot_demos/${CMAKE_BUILD_TYPE}\;${PYTHONHOME}\;${PYTHONHOME}/Scripts\;${PYTHONHOME}/Library/bin\;$ENV{PATH};PYTHONHOME=${PYTHONHOME};PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python\;${CMAKE_CURRENT_BINARY_DIR}/depends/implot_demos/${CMAKE_BUILD_TYPE}")

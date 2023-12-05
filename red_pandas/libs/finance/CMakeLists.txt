
# create LIBRARY

set(PRIVATE_npv_sources
        libs/finance/src/npv.cpp
        )
set(PUBLIC_npv_sources
        depends/date/date.h
        libs/finance/include/npv.h
        )
set(PUBLIC_npv_include_dirs
        depends/date
        libs/finance/include/
        )
set(PUBLIC_targets
        portfolio
        )
add_library(npv_static STATIC)
target_sources(npv_static PRIVATE ${PRIVATE_npv_sources} PUBLIC ${PUBLIC_npv_sources})
target_include_directories(npv_static PUBLIC ${PUBLIC_npv_include_dirs})
target_compile_features(npv_static PUBLIC cxx_std_20)
target_link_libraries(npv_static PUBLIC ${PUBLIC_targets})
set_target_properties(npv_static PROPERTIES VERSION ${PROJECT_VERSION})
install(TARGETS npv_static DESTINATION ${CMAKE_SOURCE_DIR}/bin)

# tests LIBRARY

add_executable(test_finance)
set(PRIVATE_tests_sources
        depends/Catch2/src/catch_amalgamated.cpp
        libs/finance/tests/tests_npv.cpp
        )
set(PUBLIC_tests_sources
        depends/Catch2/include/catch_amalgamated.hpp
        )
set(PUBLIC_include_dirs
        depends/Catch2/include
        )
set(PUBLIC_targets
        npv_static
        )
target_sources(test_finance PRIVATE ${PRIVATE_tests_sources} PUBLIC ${PUBLIC_tests_sources})
target_include_directories(test_finance PUBLIC ${PUBLIC_include_dirs})
target_compile_features(test_finance PUBLIC cxx_std_20)
target_link_libraries(test_finance PUBLIC ${PUBLIC_targets})

add_test(	NAME test_finance
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test_finance.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/finance/tests)
# set_tests_properties(test_finance PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;$ENV{PATH}")


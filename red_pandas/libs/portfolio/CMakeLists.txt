
# create LIBRARY

set(PRIVATE_sources
        libs/portfolio/src/portfolio.cpp
        )
set(PUBLIC_sources
        libs/portfolio/include/portfolio.h
        )
set(PUBLIC_include_dirs
        libs/portfolio/include/
        )
add_library(portfolio STATIC)
target_sources(portfolio PRIVATE ${PRIVATE_sources} PUBLIC ${PUBLIC_sources})
target_include_directories(portfolio PUBLIC ${PUBLIC_include_dirs})
target_compile_features(portfolio PUBLIC cxx_std_20)
set_target_properties(portfolio PROPERTIES VERSION ${PROJECT_VERSION})
install(TARGETS portfolio DESTINATION ${CMAKE_SOURCE_DIR}/bin)

# tests LIBRARY

add_executable(test_portfolio)
set(PRIVATE_tests_sources
        libs/portfolio/tests/test_portfolio.cpp
        depends/Catch2/src/catch_amalgamated.cpp
        )
set(PUBLIC_tests_sources
        depends/Catch2/include/catch_amalgamated.hpp
        )
set(PUBLIC_tests_include_dirs
        depends/Catch2/include
        )
set(PUBLIC_targets
        portfolio
        npv_static
        )
target_sources(test_portfolio PRIVATE ${PRIVATE_tests_sources} PUBLIC ${PUBLIC_tests_sources})
target_include_directories(test_portfolio PUBLIC ${PUBLIC_tests_include_dirs})
target_compile_features(test_portfolio PUBLIC cxx_std_20)
target_link_libraries(test_portfolio PUBLIC ${PUBLIC_targets})

add_test(	NAME test_portfolio
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test_portfolio.exe
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/portfolio/tests)
# set_tests_properties(test_portfolio PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;$ENV{PATH}")

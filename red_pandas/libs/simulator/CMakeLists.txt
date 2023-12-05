
# create LIBRARY

set(PRIVATE_sources
        libs/simulator/src/simulator.cpp
        )
set(PUBLIC_sources
        libs/simulator/include/simulator.h
        )
set(PUBLIC_include_dirs
        libs/simulator/include/
        )
set(PUBLIC_link_dirs
        )
set(PUBLIC_targets
        entityx
        portfolio
        red_pandas_static
        )
add_library(simulator STATIC)
target_sources(simulator PRIVATE ${PRIVATE_sources} PUBLIC ${PUBLIC_sources})
target_include_directories(simulator PUBLIC ${PUBLIC_include_dirs})
target_compile_features(simulator PUBLIC cxx_std_20)
target_link_directories(simulator PUBLIC ${PUBLIC_link_dirs})
target_link_libraries(simulator PUBLIC ${PUBLIC_targets})
set_target_properties(simulator PROPERTIES VERSION ${PROJECT_VERSION})
install(TARGETS simulator DESTINATION ${CMAKE_SOURCE_DIR}/bin)

# tests LIBRARY

add_executable(test_simulator)
set(PRIVATE_tests_sources
        libs/simulator/tests/test_simulator.cpp
        depends/Catch2/src/catch_amalgamated.cpp
        )
set(PUBLIC_tests_sources
        depends/Catch2/include/catch_amalgamated.hpp
        )
set(PUBLIC_tests_include_dirs
        depends/Catch2/include
        )
set(PUBLIC_targets
        simulator
        )
target_sources(test_simulator PRIVATE ${PRIVATE_tests_sources} PUBLIC ${PUBLIC_tests_sources})
target_include_directories(test_simulator PUBLIC ${PUBLIC_tests_include_dirs})
target_compile_features(test_simulator PUBLIC cxx_std_20)
target_link_libraries(test_simulator PUBLIC ${PUBLIC_targets})

add_test(	NAME test_simulator
            COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/test_simulator.exe
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/libs/simulator/tests)
# set_tests_properties(test_simulator PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;$ENV{PATH}")


# create LIBRARY

set(PRIVATE_sources
        libs/gui/src/gui.cpp
        )
set(PUBLIC_sources

        )
set(PUBLIC_include_dirs
        libs/gui/include/
        depends/pybind11/include/
        depends/implot_demos/common
        depends/implot_demos/3rdparty
        )
set(PRIVATE_targets
        fmt::fmt
        curl_int
        red_pandas_static
        app
        )
set(PUBLIC_targets
        )
add_executable(gui)
target_sources(gui PRIVATE ${PRIVATE_sources} PUBLIC ${PUBLIC_sources})
target_include_directories(gui PUBLIC ${PUBLIC_include_dirs})
target_compile_features(gui PUBLIC cxx_std_20)
set_target_properties(gui PROPERTIES VERSION ${PROJECT_VERSION})
target_link_libraries(red_pandas PRIVATE ${PRIVATE_targets})
target_link_libraries(red_pandas PUBLIC ${PUBLIC_targets})
install(TARGETS gui DESTINATION ${CMAKE_SOURCE_DIR}/bin)

# test stocks
add_test(
        NAME test_gui
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/gui.exe
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
set_tests_properties(test_gui PROPERTIES ENVIRONMENT "PATH=${THIRD_LIB}\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}\;${PYTHONHOME}\;${PYTHONHOME}/Scripts\;${PYTHONHOME}/Library/bin\;$ENV{PATH};PYTHONHOME=${PYTHONHOME};PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}/libs/core/python\;${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}")

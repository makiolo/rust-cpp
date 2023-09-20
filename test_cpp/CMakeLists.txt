cmake_minimum_required(VERSION 3.25)
project(test_cpp)

set(CMAKE_CXX_STANDARD 17)

# enable_language(Rust)
# include(CMakeCargo)

add_library(test_cpp STATIC main.cpp)

install(TARGETS test_cpp DESTINATION ${CMAKE_SOURCE_DIR}/bin)

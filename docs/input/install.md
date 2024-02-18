# Install

## Requirements:
- CMake (>= 3.23), Python 3 and SWIG 4 in PATH.
- Check are availables hardcoded paths in CMakeLists.txt (C:\Miniconda3, etc ...)





## Generate solution (Visual Studio, Linux Makefile ...)
    - mkdir build
    - cd build

Windows

    - cmake .. -G"Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
    - cmake .. -G"Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug

Linux

    - cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    - cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug

Note1:

    - Can check other available generators with cmake -h

Note2:

    - Can force cmake re-generation removing "CMakeCache.txt"

## Compile
Check your working directory is "build".

    - cmake --build . --config Debug
    - cmake --build . --config Release

## Run tests
Check your working directory is "build".

    - ctest . -C Debug
    - ctest . -C Release

Note: I am experimenting to change to "pytest" like launcher of tests.

## Deploy (Install in python)
Check your working directory is "build".

    - cmake --build . --config Debug --target INSTALL
    - cmake --build . --config Release --target INSTALL

Note: Recommend deploy is use setup.py/pip, but cmake is a good workaround.

## Hola
dassdsddasdasdadsdassddsa
dasdsadsdasas
- asdaddas
- dasdassdsa
- adsdasdasasd
asdadssadsad



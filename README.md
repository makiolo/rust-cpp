# rust-cpp
Minimal web server in Rust calling to C++ in your handler

# maths

https://www.youtube.com/watch?v=JU7fMhHr2L8

# build rust-cpp

setup:
```
cmake .. -G Ninja -DCMAKE_C_COMPILER="C:/dev/llvm-project/build/Release/bin/clang.exe" -DCMAKE_CXX_COMPILER="C:/dev/llvm-project/build/Release/bin/clang.exe" -DCMAKE_LINKER="C:/dev/llvm-project/build/Release/bin/lld-link.exe"
```
build:
```
cmake --build . --config Release
```
test:
```
ctest . --config Release
```

# build clang

```
git clone https://github.com/llvm/llvm-project.git

cmake -DLLVM_ENABLE_PROJECTS=clang -G "Visual Studio 17 2022" -A x64 -Thost=x64 ..\llvm

cmake -DLLVM_ENABLE_PROJECTS=lld -G "Visual Studio 17 2022" -A x64 -Thost=x64 ..\llvm

cmake -DLLVM_ENABLE_PROJECTS=“clang;lld;lldb” -G "Visual Studio 17 2022" -A x64 -Thost=x64 ..\llvm
```

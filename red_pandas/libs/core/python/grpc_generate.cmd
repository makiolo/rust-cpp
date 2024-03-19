@echo off

setlocal

:: https://github.com/grpc-ecosystem/awesome-grpc#tools

SET mypath=%~dp0
:: set PATH="%mypath%..\..\..\depends\protoc-25.3-win64\bin\";%PATH%
set PATH="D:\dev\vcpkg_root\packages\protobuf_x64-windows\tools\protobuf";%PATH%

python -m grpc_tools.protoc --proto_path=. --python_out=. --pyi_out=. --grpc_python_out=. grpc.proto
protoc --proto_path=. --plugin=grpc_plugin --cpp_out=. grpc.proto

endlocal

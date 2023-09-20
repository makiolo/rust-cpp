@echo off

python -m grpc_tools.protoc --python_out=grpc/ --pyi_out=grpc/ --grpc_python_out=grpc/ grpc.proto

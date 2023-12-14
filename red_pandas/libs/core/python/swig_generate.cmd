@echo off

SET mypath=%~dp0
SET PATH=C:\dev\swigwin-4.0.2;%PATH%
SET PATH=D:\swigwin-4.1.1;%PATH%

:: cd %mypath%\java
:: swig -Wall -java -c++ -I%mypath%/core/include -module red_pandas -outcurrentdir %mypath%\java\interface_py.i

:: '-jsc', '-v8' or '-node'
:: mkdir %mypath%\javascript
:: cd %mypath%\javascript
:: swig -Wall -javascript -node -c++ -I%mypath% -I%mypath%/../include -I%mypath%/../../finance/include -I%mypath%/../../portfolio/include -I%mypath%/../../simulator/include -module red_pandas -outcurrentdir %mypath%/javascript/interface_py.i

cd %mypath%
swig -Wall -python -c++ -globals static -I%mypath% -I%mypath%/../include -I%mypath%/../../finance/include -I%mypath%/../../portfolio/include -I%mypath%/../../simulator/include -module red_pandas -outcurrentdir %mypath%/interface_py.i

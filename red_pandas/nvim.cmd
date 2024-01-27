@echo off

:: isolation
set PATH=C:\dev\sandbox\bin;C:\dev\zig;C:\dev\cmake-3.27.1-windows-x86_64\bin;C:\nvim-win64\bin;C:\Program Files\Git\cmd;C:\Windows\System32
set PYTHONHOME=;

:: change to python 3 (64)
set "PYTHONHOME=C:\Miniconda3"
set "PATH=%PATH:C:\Python27=C:\Miniconda3%"
set "PATH=%PATH:C:\Python310=C:\Miniconda3%"
set "PATH=%PATH:C:\Miniconda2=C:\Miniconda3%"
set PATH=%PYTHONHOME%;%PATH%
set PATH=%PYTHONHOME%\Scripts;%PATH%
set PATH=%PYTHONHOME%\Library\bin;%PATH%

SET mypath=%~dp0
SET script_path=%mypath:~0,-1%

cd /d %script_path%
cmd /c C:\nvim-win64\bin\nvim.exe

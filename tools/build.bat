@echo off

set originalDir=%CD%
set scriptDir=%~dp0
set projectRoot=%scriptDir%\..

cd "%projectRoot%\bin" >nul 2>nul
del Infecdead.exe

IF NOT EXIST "%projectRoot%\build" mkdir "%projectRoot%\build"
cd "%projectRoot%\build"

cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ..
cmake --build .

echo.
cd "%projectRoot%\bin"
Infecdead.exe

cd %originalDir%

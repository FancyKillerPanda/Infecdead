@echo off

set originalDir=%CD%
set scriptDir=%~dp0
set projectRoot=%scriptDir%\..

cd "%projectRoot%\bin" >nul 2>nul && del Infecdead.exe

IF NOT EXIST "%projectRoot%\build" mkdir "%projectRoot%\build"
cd "%projectRoot%\build"

cmake -G "Visual Studio 17 2022" -T ClangCL -A x64 ..
cmake --build .

echo.
cd "%projectRoot%\bin\Debug\"
Infecdead.exe

cd %originalDir%

@echo off
setlocal
cd /d "%~dp0"
if not exist build mkdir build
cmake -S . -B build -G "MinGW Makefiles"
if errorlevel 1 pause & exit /b 1
cmake --build build
if errorlevel 1 pause & exit /b 1
build\felsk_api.exe
pause

@echo off
cd /d "%~dp0"

if exist build (
    rmdir /s /q build
)

cmake -S . -B build -G "Ninja"
if errorlevel 1 pause & exit /b 1

cmake --build build
if errorlevel 1 pause & exit /b 1

build\felsk_api.exe
pause

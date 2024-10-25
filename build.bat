REM made by a Linux user & Chat Gippity so quality not guaranteed
REM fix if fails

@echo off

REM Create build directory
if not exist "build" (
    mkdir build
)

REM Run CMake
cmake -B build .

REM Build the project using -C flag
make -C build ./bin/main

REM Run the executable
build\bin\main.exe

echo Build completed.


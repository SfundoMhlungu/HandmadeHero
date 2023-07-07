@echo off

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM Compiling C++ file with MinGW
 zig c++ ..\handmade\src\win32_handmade.cpp -lgdi32 -lXInput1_4 -ldsound  -o main.exe

popd

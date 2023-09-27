@echo off

if not exist ..\..\build mkdir ..\..\build
pushd ..\..\build

REM Compiling C++ file with g++
REM -Wno-writable-strings -Wno-unused-parameter - turn off warnings about writable strings and unused parameters
g++ -g -Wall -Wextra -Wno-writable-strings -Wno-unused-parameter ..\handmade\src\win32_handmade.cpp -lgdi32 -lXInput1_4 -ldsound -o main.exe

popd

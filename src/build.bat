@echo off

IF NOT EXIST ..\..\build mkdir ..\..\build
pushd ..\..\build

REM Compiling C++ file with MinGW
REM -Wno-writable-strings -Wno-unused-parameter - - turn off warning 
REM  implicit sign conversion is taking place, potentially resulting in data loss
 zig c++ -g -W -Wconversion -Wno-sign-conversion ..\handmade\src\win32_handmade.cpp -lgdi32 -lXInput1_4 -ldsound  -o main.exe

popd

@echo off

if not exist "build" mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" || exit /b 1
cmake --build . || exit \b 1
ctest || exit \b 1

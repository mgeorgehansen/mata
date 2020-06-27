@echo off

if not exist "build" mkdir build
cd build
cmake -G "Visual Studio 15 2017" -DCMAKE_TOOLCHAIN_FILE="%VCPKG_HOME%\scripts\buildsystems\vcpkg.cmake" .. || exit /b 1
cmake --build . || exit \b 1
ctest -C Release || exit \b 1

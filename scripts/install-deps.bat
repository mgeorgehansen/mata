@echo off

setlocal

if not defined VCPKG_HOME set "VCPKG_HOME=%USERPROFILE%\vcpkg"
set VCPKG="%VCPKG_HOME%\vcpkg.exe"
echo "using VCPKG_HOME=%VCPKG_HOME%"

if not exist %VCPKG% (
  echo "vcpkg not found at $VCPKG, fetching vcpkg and building..."
  set cwd="%cd%"
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git "%VCPKG_HOME%" || exit /b 1
  cd "%VCPKG_HOME%"
  ./bootstrap-vcpkg.bat || exit /b 1
  cd "%cwd%"
)

"%VCPKG%" install fmt glfw3 glbinding

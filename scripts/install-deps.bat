@echo off

setlocal

REM Install Mesa3d

powershell -Command "Invoke-WebRequest https://github.com/pal1000/mesa-dist-win/releases/download/20.1.2/mesa3d-20.1.2-release-msvc.7z -OutFile mesa3d-20.1.2-release-msvc.7z"
7z e mesa3d-20.1.2-release-msvc.7z -omesa3d -r -y
(
  echo "3"
  echo "8"
)> "cmd.tmp"
if not errorlevel 1 mesa3d\systemwidedeploy.cmd < "cmd.tmp"
del "cmd.tmp"


REM Install VCPKG and 3rd party libraries

if not defined VCPKG_HOME set "VCPKG_HOME=%USERPROFILE%\vcpkg"
set VCPKG="%VCPKG_HOME%\vcpkg.exe"
echo "using VCPKG_HOME=%VCPKG_HOME%"

set cwd="%cd%"
cd "%VCPKG_HOME%"

git rev-parse --is-inside-work-tree
if errorlevel 1 (
  echo "vcpkg source not found, cloning vcpkg..."
  git clone --depth=1 --branch 2020.06 https://github.com/Microsoft/vcpkg.git . || exit /b 1
)

if not exist %VCPKG% (
  echo "vcpkg binary not found at %VCPKG%, building vcpkg..."
  bootstrap-vcpkg.bat || exit /b 1
)

cd "%cwd%"

echo "installing deps..."
%VCPKG% install fmt glfw3 glbinding catch2 || exit /b 1

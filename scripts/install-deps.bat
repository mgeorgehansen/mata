setlocal

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

cd "%cwd%" || exit /b 1

echo "installing deps..."
%VCPKG% install fmt glfw3 glbinding catch2 || exit /b 1

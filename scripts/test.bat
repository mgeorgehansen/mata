cd build/ || exit /b 1
ctest -C Debug --output-on-failure --headless || exit /b 1

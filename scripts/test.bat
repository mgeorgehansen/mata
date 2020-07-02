@echo off

ctest -C Debug --output-on-failure --headless || exit \b 1

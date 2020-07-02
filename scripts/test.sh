#!/bin/bash

cd build/ || exit 1
ctest --output-on-failure --headless || exit 1

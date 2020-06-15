cmake_minimum_required(VERSION 3.4)
project(mata)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(mata
    src/main.cpp
    src/renderer.cpp
    src/glad.c
)
target_compile_features(mata PUBLIC cxx_std_14)
target_include_directories(mata PRIVATE include)
target_link_libraries(mata ${CONAN_LIBS})

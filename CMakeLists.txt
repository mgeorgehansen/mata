cmake_minimum_required(VERSION 3.4)
project(mata)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glbinding CONFIG REQUIRED)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)

add_executable(mata
    src/main.cpp
    src/virtual-file-system.cpp
    src/renderer.cpp
    src/app.cpp
)
target_compile_features(mata PUBLIC cxx_std_17)
target_include_directories(mata PRIVATE include)
target_link_libraries(mata PRIVATE glfw glbinding::glbinding)

file(
    COPY ${CMAKE_SOURCE_DIR}/resources
    DESTINATION ${CMAKE_BINARY_DIR}
)

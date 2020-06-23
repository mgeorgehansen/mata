cmake_minimum_required(VERSION 3.4)
project(mata)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glbinding CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_compile_definitions($<$<CONFIG:Debug>:DEBUG>)
# Based on https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md#compilers
if (MSVC)
    add_compile_options(
      /WX /permissive /W4 /w14640 /w14242 /w14254 /w14263 /w14265
      /w14287 /we4289 /w14296 /w14311 /w14545 /w14546 /w14547
      /w14549 /w14555 /w14619 /w14640 /w14826 /w14905 /w14906
      /w14928
    )
else()
    add_compile_options(-Werror -Weverything -Wno-c++98-compat -Wno-padded)
endif()

add_executable(mata
    src/main.cpp
    src/utils/filesystem.cpp
    src/virtual-file-system.cpp
    src/renderer.cpp
    src/app.cpp
)
target_compile_features(mata PUBLIC cxx_std_17)
target_include_directories(mata PRIVATE include)
target_link_libraries(mata PRIVATE glfw glbinding::glbinding fmt::fmt)

file(
    COPY ${CMAKE_SOURCE_DIR}/resources
    DESTINATION ${CMAKE_BINARY_DIR}
)

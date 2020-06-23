FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.1)
FetchContent_MakeAvailable(catch2)

add_executable(mata_test test.cpp)
target_compile_features(mata_test PRIVATE cxx_std_17)

target_link_libraries(mata_test PRIVATE mata_lib Catch2::Catch2)

add_test(NAME mata_test COMMAND mata_test)

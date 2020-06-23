add_executable(mata mata.cpp)
target_link_libraries(mata PRIVATE mata_lib)

file(COPY ${CMAKE_SOURCE_DIR}/resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

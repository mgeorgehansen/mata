file(GLOB_RECURSE HEADER_LIST CONFIGURE_DEPENDS
     "${PROJECT_SOURCE_DIR}/include/mata/*.hpp")
file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

add_library(mata_lib ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(mata_lib PUBLIC ../include)
target_link_libraries(mata_lib PRIVATE std::filesystem glfw
                                       glbinding::glbinding fmt::fmt)

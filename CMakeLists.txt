cmake_minimum_required(VERSION 3.15)
project(cyberpolar)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(LIB_SRC
    src/log.cpp
)

add_library(cyberpolar SHARED ${LIB_SRC})

add_executable(test tests/test.cpp)
add_dependencies(test cyberpolar)
target_link_libraries(test cyberpolar)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
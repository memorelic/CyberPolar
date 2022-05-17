cmake_minimum_required(VERSION 3.15)

# 设置C++标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(cyberpolar LANGUAGES CXX VERSION 0.0.1)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (WIN32)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
endif()

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

add_executable(test)
file(GLOB testsrc CMAKE_CONFIGURE_DEPENDS ./test/*.cpp ./test/*.h)
target_sources(test PUBLIC ${testsrc})

add_subdirectory(./src/Log)


add_dependencies(test cyberpolar)
target_link_libraries(test cyberpolar)


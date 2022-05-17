add_library(Log SHARED)

add_definitions(-DLOG_DLL -DLOG_WINDOWS)

file(GLOB sources CONFIGURE_DEPENDS ./src/*.cpp ./src/*.h)
target_sources(Log ${sources})

target_include_directories(Log PUBLIC ./include)

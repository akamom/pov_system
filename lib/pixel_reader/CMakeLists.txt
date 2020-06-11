add_library(
    pixel_reader
    src/hello.cpp
    src/hello.h
)

target_include_directories(pixel_reader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
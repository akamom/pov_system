add_library(
    pixel_reader
    src/pixel_reader.cpp
    src/pixel_reader.h
    src/lodepng.cpp
    src/lodepng.h
)

target_include_directories(pixel_reader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
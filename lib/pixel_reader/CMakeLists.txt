add_library(
    pixel_reader SHARED
    src/hello.cpp
    src/hello.h
)

target_link_libraries(pixel_reader PRIVATE lodepng)
target_include_directories(pixel_reader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../lodepng/src")

target_include_directories(pixel_reader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
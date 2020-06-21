cmake_minimum_required(VERSION 3.17)
project(pov_system VERSION 1.0)

add_subdirectory(lib/lodepng)
add_subdirectory(lib/pixel_reader)

add_executable(pov_system main.cpp)

target_link_libraries(pov_system PRIVATE pixel_reader)
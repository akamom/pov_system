cmake_minimum_required(VERSION 3.17)
project(pov_system VERSION 1.0)

add_subdirectory(lib/pixel_reader)

add_executable(pov_system main.cpp)

add_library(
    imageselection
    imageselection.cpp
    imageselection.h
)

target_link_libraries(imageselection PRIVATE pixel_reader)
target_link_libraries(pov_system PUBLIC imageselection)

add_library(
    filesys
    filesys.cpp
    filesys.h
)

target_link_libraries(pov_system PRIVATE filesys)

add_library(
    stringop
    stringop.cpp
    stringop.h
)

target_link_libraries(pov_system PUBLIC stringop)

add_library(
    commandparser
    commandparser.cpp
    commandparser.h
)

target_link_libraries(commandparser PUBLIC stringop)
target_link_libraries(pov_system PUBLIC commandparser)
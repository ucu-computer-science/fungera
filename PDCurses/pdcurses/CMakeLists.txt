cmake_minimum_required(VERSION 3.2)

file(GLOB SRC "*.c" "../wincon/*.c")
add_library(pdcurses ${SRC})
target_include_directories(pdcurses PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
cmake_minimum_required(VERSION 3.2)

file(GLOB_RECURSE SRC "pdcurses/*.c" "pdcurses/*.h" "wincon/*.c" "wincon/*.h")

add_library(PDCurses ${SRC})
target_include_directories(PDCurses PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
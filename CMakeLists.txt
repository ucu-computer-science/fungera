cmake_minimum_required(VERSION 3.16)
project(cppfungera)

set(CMAKE_CXX_STANDARD 14)

add_executable(cppfungera main.cpp)

include_directories(PDCurses)

add_subdirectory(PDCurses/pdcurses)
target_link_libraries(cppfungera PRIVATE pdcurses)
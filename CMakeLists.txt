cmake_minimum_required(VERSION 3.16)
project(cppfungera)

set(CMAKE_CXX_STANDARD 14)

add_executable(cppfungera main.cpp Cell.h Memory.cpp Memory.h Organism.cpp Organism.h Queue.h App.cpp App.h)

include_directories(PDCurses)

add_subdirectory(PDCurses/pdcurses)
target_link_libraries(cppfungera PRIVATE pdcurses)
cmake_minimum_required(VERSION 3.16)
project(cppfungera)

set(CMAKE_CXX_STANDARD 14)

add_executable(cppfungera src/main.cpp src/Cell.h src/Memory.cpp src/Memory.h src/Organism.cpp src/Organism.h src/Queue.h src/App.cpp src/App.h)

include_directories(libs/PDCurses)

#add_subdirectory(libs/PDCurses/pdcurses)
target_link_libraries(cppfungera PRIVATE ncurses)
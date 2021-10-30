cmake_minimum_required(VERSION 3.16)
project(cppfungera)

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")

add_executable(cppfungera ${SRC})

if(WIN32 OR MSVC)
    add_subdirectory(libs/PDCurses)
    target_link_libraries(cppfungera PRIVATE PDCurses)
else()
    # cmake -G"Unix Makefiles" -DCMAKE_PREFIX_PATH=/usr/lib/x86_64-linux-gnu/libncurses.so ..
    # 	find_library(ncurses REQUIRED)
    target_link_libraries(cppfungera PRIVATE ncurses)
endif()

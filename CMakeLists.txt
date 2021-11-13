cmake_minimum_required(VERSION 3.16)
project(cppfungera)

find_package(Boost REQUIRED serialization program_options)
include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")

add_executable(cppfungera ${SRC})

if(WIN32 OR MSVC)
    add_subdirectory(libs/PDCurses)
    target_link_libraries(cppfungera PRIVATE PDCurses)
    target_link_libraries(cppfungera ${Boost_LIBRARIES})
else()
    target_link_libraries(cppfungera PRIVATE ncurses ${Boost_LIBRARIES})
endif()


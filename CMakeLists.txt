cmake_minimum_required(VERSION 3.5)

project(qtfungera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(MAIN_EXECUTABLE_NAME qtfungera)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(ENABLE_PVS_STUDIO OFF)

#  Only one of Memory (MSAN), Address (ASAN), or Thread (TSan)
#  sanitizers is applicable at the time -- the first defined.

#! UndefinedBehaviorSanitizer (UBSan)
#  Info: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html
set(ENABLE_UBSan ON)
#! AddressSanitizer -- detects use after free or after scope exit, memory overflows and leaks.  
#  Info: https://github.com/google/sanitizers/wiki/AddressSanitizer
set(ENABLE_ASAN ON)
#! ThreadSanitizer -- detects data races.
set(ENABLE_TSan OFF)
#! MemorySanitizer detects uninitialized memory reads 
#  Info: https://github.com/google/sanitizers/wiki/MemorySanitizer
set(ENABLE_MSAN OFF)

# Warnings as errors should be imported here -- do not move this line
include(cmake/CompilerWarnings.cmake)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost REQUIRED serialization program_options)
include_directories(${Boost_INCLUDE_DIR})

set(PROJECT_SOURCES
        src/main.cpp
#        mainwindow.cpp
#        mainwindow.h
        src/point.h
        src/point.cpp
        src/cell.h
        src/memory.h
        src/memory.cpp
        src/memoryview.h
        src/memoryview.cpp
        src/organism.h
        src/organism.cpp
        src/organismqueue.h
        src/organismqueue.cpp
        src/statuspanel.h
        src/statuspanel.cpp
        src/statistics.cpp
        src/statistics.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${MAIN_EXECUTABLE_NAME}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ${MAIN_EXECUTABLE_NAME} APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(${MAIN_EXECUTABLE_NAME} SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(${MAIN_EXECUTABLE_NAME}
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(${MAIN_EXECUTABLE_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${Boost_LIBRARIES})

# target_link_libraries(${MAIN_EXECUTABLE_NAME} LINK_PUBLIC ${Boost_LIBRARIES})

set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER ${MAIN_EXECUTABLE_NAME}_ID
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${MAIN_EXECUTABLE_NAME})
endif()

# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${MAIN_EXECUTABLE_NAME})

# Include CMake setup
include(cmake/main-config.cmake)


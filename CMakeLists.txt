cmake_minimum_required(VERSION 3.5)

project(qtfungera VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

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
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtfungera
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET qtfungera APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(qtfungera SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(qtfungera
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(qtfungera PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${Boost_LIBRARIES})

# target_link_libraries(qtfungera LINK_PUBLIC ${Boost_LIBRARIES})

set_target_properties(qtfungera PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

target_compile_features(qtfungera PRIVATE cxx_std_17)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(qtfungera)
endif()

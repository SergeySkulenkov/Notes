cmake_minimum_required(VERSION 3.5)

project(untitled3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

cmake_policy(SET CMP0115 NEW)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        version.h
        checker.h
        checker.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(Demo
    ${PROJECT_SOURCES}
)


target_link_libraries(Demo
        PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        PRIVATE HunspellLib
)

install(TARGETS Demo DESTINATION demo)

set_target_properties(Demo PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if (WIN32)
    add_custom_command(TARGET Demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/../HunspellLib/libHunspellLib.dll"
            $<TARGET_FILE_DIR:Demo>)

    add_custom_command(TARGET Demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/dictionaries/ru_RU.dic"
                $<TARGET_FILE_DIR:Demo>)
    add_custom_command(TARGET Demo POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/dictionaries/ru_RU.aff"
                $<TARGET_FILE_DIR:Demo>)

endif (WIN32)



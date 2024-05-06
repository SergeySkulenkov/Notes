cmake_minimum_required(VERSION 3.5)

project(NotesProject VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        widget.cpp
        stylehelper.cpp
        stylehelper.h
        widget.h
        widget.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(NotesProject
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET NotesProject APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(NotesProject SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(NotesProject
            ${PROJECT_SOURCES}
            assets/themes/standart.json
        )
    endif()
endif()

target_link_libraries(NotesProject PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.NotesProject)
endif()
set_target_properties(NotesProject PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS NotesProject
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(NotesProject)
endif()
macro(copy_directory src dst)
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${src} ${dst}
        RESULT_VARIABLE ret
        OUTPUT_VARIABLE out
        ERROR_VARIABLE  err
    )
    if(ret)
        message(FATAL_ERROR "Could not be copied: ${err}")
    endif()
endmacro()
copy_directory(${PROJECT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/assets/themes/standart.json
        ${CMAKE_CURRENT_BINARY_DIR}/assets/themes/standart.json COPYONLY)

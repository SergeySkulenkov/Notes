cmake_minimum_required(VERSION 3.5)

project(NotesProject)

set (CMAKE_CXX_STANDARD 17)

add_subdirectory(HunspellLib)
add_subdirectory(Notes)

# USE_FOLDERS group cmake generated projects into one (CMakePredefinedTargets) folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Below line set Demo as startup application in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DemoApp)







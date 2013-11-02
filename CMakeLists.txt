CMAKE_MINIMUM_REQUIRED (VERSION 2.8.8)
# Dense kitchen project
PROJECT(ExNCo)

IF(APPLE)
    MESSAGE( STATUS "Apple system" )
    SET (CMAKE_CXX_FLAGS "-stdlib=libc++ -std=c++11")
    INCLUDE_DIRECTORIES( /System/Library/Frameworks )
ENDIF()

IF(WIN32)
    MESSAGE( STATUS "Windows" )
    IF(MINGW)
        SET (CMAKE_CXX_FLAGS "-std=c++11")
    ELSEIF(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    ENDIF()

    cmake_policy(SET CMP0020 NEW)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Load external libraries
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)

# Include
INCLUDE_DIRECTORIES(
${Qt5Core_INCLUDE_DIRS}
${Qt5Gui_INCLUDE_DIRS}
${Qt5Widgets_INCLUDE_DIRS}
${Qt5OpenGL_INCLUDE_DIRS}
)

# Load project libs
ADD_SUBDIRECTORY(AI)
ADD_SUBDIRECTORY(EntitySystem)
ADD_SUBDIRECTORY(Graphics)
ADD_SUBDIRECTORY(Physics)

# Binaries
ADD_EXECUTABLE(ExNCo main.cpp)

# Link to libraries
TARGET_LINK_LIBRARIES (ExNCo
    AI
    EntitySystem
    Graphics
    Physics
    ${OPENCV_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# Activate Qt for project
# Must be the last line in this file, else { FAIL }
qt5_use_modules(ExNCo Core Gui Widgets OpenGL)

# This command copies all models, shaders etc. but during development it doesn't
# update the files when changed..
#add_custom_command(
#    TARGET ExNCo
#    PRE_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/Graphics/Models ${CMAKE_BINARY_DIR}/Graphics/Models
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Graphics/Textures ${CMAKE_BINARY_DIR}/Graphics/Textures
#    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Graphics/Shaders ${CMAKE_BINARY_DIR}/Graphics/Shaders
#)

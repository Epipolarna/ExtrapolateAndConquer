CMAKE_MINIMUM_REQUIRED (VERSION 2.8.8)
#extrapolate and conquer game
PROJECT(ExNCo)

IF(APPLE)
    MESSAGE( STATUS "Apple system" )
    SET (CMAKE_CXX_FLAGS "-stdlib=libc++ -std=c++11 -pedantic")
    INCLUDE_DIRECTORIES( /System/Library/Frameworks )
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
ENDIF()

IF(WIN32)
    MESSAGE( STATUS "Windows" )
    IF(MINGW)
        SET (CMAKE_CXX_FLAGS "-std=c++11 -pedantic")
    ELSEIF(MSVC)
        add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    ENDIF()
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
    cmake_policy(SET CMP0020 NEW)

    MESSAGE(STATUS "OPENCV_MINGW_X86_INSTALL_DIR: " $ENV{OPENCV_MINGW_X86_INSTALL_DIR})
ENDIF()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Load external libraries
find_package(Qt5 COMPONENTS Core Gui Widgets OpenGL REQUIRED)
find_package(OpenCV REQUIRED HINTS $ENV{OPENCV_MINGW_X86_INSTALL_DIR})
find_package(OpenGL REQUIRED)

MESSAGE( STATUS "OPENCV_LIB_DIR: " ${OpenCV_LIB_DIR} )
MESSAGE( STATUS "OPENCV_LIBS: " ${OpenCV_LIBS} )

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
ADD_SUBDIRECTORY(Components)
ADD_SUBDIRECTORY(Systems)

# Binaries
ADD_EXECUTABLE(ExNCo main.cpp ExtrapolateAndConquer.cpp ResourceManager.cpp)

# Link to libraries
TARGET_LINK_LIBRARIES (ExNCo
    AI
    EntitySystem
    Graphics
    Physics
    Components
    Systems
    ${OPENGL_LIBRARIES}
    ${OpenCV_LIBS}
)

# Activate Qt for project
# Must be the last line in this file, else { FAIL }
qt5_use_modules(ExNCo Core Gui Widgets OpenGL)

FILE(GLOB_RECURSE shaders resources/shaders/*)
FILE(GLOB_RECURSE textures resources/textures/*)
FILE(GLOB_RECURSE models resources/models/*)

#FILE(COPY ${shaders} DESTINATION resources/shaders/)
#FILE(COPY ${models} DESTINATION resources/models/)
#FILE(COPY ${textures} DESTINATION resources/textures/)

CMAKE_MINIMUM_REQUIRED (VERSION 2.8.8)
# Dense kitchen project
PROJECT(ExNCo)

# is TRUE on Apple OS X
MESSAGE( STATUS "APPLE: " ${APPLE} )
# is TRUE when using the MinGW compiler in Windows
MESSAGE( STATUS "MINGW: " ${MINGW} )

IF(APPLE)
    # Apple specific
    SET (CMAKE_CXX_FLAGS "-stdlib=libc++ -std=c++11")
ENDIF()

IF(WIN32)
    # Windows specific
    IF(MINGW)
        SET (CMAKE_CXX_FLAGS "-std=c++11 -Wall")
    ENDIF()
ENDIF()

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#move the configurationfile to the build dir
# Shaders
CONFIGURE_FILE(Shaders/phong.vert Shaders/phong.vert COPYONLY)
CONFIGURE_FILE(Shaders/phong.frag Shaders/phong.frag COPYONLY)
CONFIGURE_FILE(Shaders/FBO/threshold.vert Shaders/FBO/threshold.vert COPYONLY)
CONFIGURE_FILE(Shaders/FBO/threshold.vert Shaders/FBO/threshold.frag COPYONLY)
CONFIGURE_FILE(Shaders/plaintextureshader.vert Shaders/plaintextureshader.vert COPYONLY)
CONFIGURE_FILE(Shaders/plaintextureshader.frag Shaders/plaintextureshader.frag COPYONLY)
# Models
CONFIGURE_FILE(Models/monkey.obj Models/monkey.obj COPYONLY)
CONFIGURE_FILE(Models/monkey.mtl Models/monkey.mtl COPYONLY)
CONFIGURE_FILE(Models/fbosquare.obj Models/fbosquare.obj COPYONLY)
CONFIGURE_FILE(Shaders/phong.vert Shaders/phong.vert COPYONLY)
CONFIGURE_FILE(Shaders/phong.vert Shaders/phong.vert COPYONLY)


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
${OPENGL_INCLUDE_DIRS}
)

# Load project libs
ADD_SUBDIRECTORY(
Modules
)

# Binaries
ADD_EXECUTABLE(ExNCo main.cpp)

# Link to libraries
TARGET_LINK_LIBRARIES (ExNCo
    Modules
    ${OPENCV_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

# Activate Qt for project
# Must be the last line in this file, else { FAIL }
qt5_use_modules(ExNCo Core Gui Widgets OpenGL)

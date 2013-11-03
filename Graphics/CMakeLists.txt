#Debug module
#this module is magical.

#add_subdirectory(Shaders)
#add_subdirectory(Models)

add_library(Graphics GraphicsWindow.cpp GraphicsWindow.ui GLWidget.cpp glm.cpp Model.cpp Object.cpp Camera.cpp TerrainGenerator.cpp)

qt5_wrap_ui(ui_GraphicsWindow.h GraphicsWindow.ui)

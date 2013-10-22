#Debug module
#this module is magical.

add_library(Graphics GraphicsWindow.cpp GraphicsWindow.ui GLWidget.cpp glm.cpp)

qt5_wrap_ui(ui_GraphicsWindow.h GraphicsWindow.ui)

#Debug module
#this module is magical.

add_library(Modules glMainWindow.cpp glMainWindow.ui GLWidget.cpp Model.cpp Object.cpp Player.cpp glm.cpp)

qt5_wrap_ui(ui_glMainWindow.h glMainWindow.ui)

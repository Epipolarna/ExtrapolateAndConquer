add_library(Graphics GraphicsWindow.cpp GraphicsWindow.ui
            Object.cpp Camera.cpp TerrainGenerator.cpp
            simplexnoise1234.cpp Model.cpp GraphicsWidget.cpp Renderer.cpp)

qt5_wrap_ui(ui_GraphicsWindow.h GraphicsWindow.ui)

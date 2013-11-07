add_library(Graphics GraphicsWindow.cpp GraphicsWindow.ui GLWidget.cpp 
			glm.cpp Model.cpp Object.cpp Camera.cpp TerrainGenerator.cpp 
                        simplexnoise1234.cpp ModelLoader.cpp GraphicsWidget.cpp Renderer.cpp)

qt5_wrap_ui(ui_GraphicsWindow.h GraphicsWindow.ui)

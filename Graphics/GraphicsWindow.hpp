#ifndef GRAPHICSWINDOW_HPP
#define GRAPHICSWINDOW_HPP

#include <QMainWindow>
#include <QKeyEvent>

#include "GraphicsWidget.hpp"
#include "Camera.hpp"

namespace Ui {
class GraphicsWindow;
}

class GraphicsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicsWindow(QWidget *parent = 0);
    ~GraphicsWindow();

    Renderer* getRenderer(void);
    void registerEventManager(graphics::Camera *c);
    
private:
    Ui::GraphicsWindow *ui;

    graphics::Camera *cam;

    GraphicsWidget* glWidget;
    
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
};

#endif // GRAPHICSWINDOW_HPP

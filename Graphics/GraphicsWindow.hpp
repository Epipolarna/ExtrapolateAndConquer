#ifndef GRAPHICSWINDOW_HPP
#define GRAPHICSWINDOW_HPP

#include <QMainWindow>
#include <QKeyEvent>

#include "GraphicsWidget.hpp"

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
private:
    Ui::GraphicsWindow *ui;

    GraphicsWidget* glWidget;
    
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void closeEvent(QCloseEvent *e);
    void initalize();
};

#endif // GRAPHICSWINDOW_HPP

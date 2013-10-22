#ifndef GRAPHICSWINDOW_HPP
#define GRAPHICSWINDOW_HPP

#include <QMainWindow>
#include "GLWidget.hpp"

namespace Ui {
class GraphicsWindow;
}

class GraphicsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GraphicsWindow(QWidget *parent = 0);
    ~GraphicsWindow();

private:
    Ui::GraphicsWindow *ui;

    GLWidget* glWidget;
};

#endif // GRAPHICSWINDOW_HPP

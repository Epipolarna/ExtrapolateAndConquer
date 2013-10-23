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
    void keyPressEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);
};

#endif // GRAPHICSWINDOW_HPP

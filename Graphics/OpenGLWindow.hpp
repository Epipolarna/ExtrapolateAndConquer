#ifndef OPENGLWINDOW_HPP
#define OPENGLWINDOW_HPP

#include <QApplication>
#include <QWindow>
#include <QOpenGLContext>
#include <QOpenGLFunctions>

#include "Renderer.hpp"

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    OpenGLWindow(QOpenGLContext *context, QScreen *screen = 0);

    void initialize();
    void registerEventManager(Camera* _camera);
    void update();

    void setHostApplication(QApplication *_application);
    void setRenderer(Renderer *value);
    Renderer* getRenderer();

protected slots:
    void resizeGl();
    void paintGl();

private:
    QApplication* application;
    QOpenGLContext* context;
    Renderer* renderer;
    Camera* camera;

    bool trackMouse;
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
};

#endif // OPENGLWINDOW_HPP

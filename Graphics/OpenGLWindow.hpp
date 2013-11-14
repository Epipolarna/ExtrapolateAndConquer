#ifndef OPENGLWINDOW_HPP
#define OPENGLWINDOW_HPP

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


    Renderer *getRenderer() const;
    void setRenderer(Renderer *value);


protected slots:
    void resizeGl();
    void paintGl();

private:
    QOpenGLContext* context;
    Renderer* renderer;
    Camera* camera;

    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
};

#endif // OPENGLWINDOW_HPP

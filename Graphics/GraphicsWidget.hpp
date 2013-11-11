#ifndef GRAPHICS_WIDGET_H
#define GRAPHICS_WIDGET_H

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>

#include "Renderer.hpp"

namespace Ui {
class GraphicsWidget;
}

class GraphicsWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit GraphicsWidget(QGLFormat format, QWidget *parent = 0);
    ~GraphicsWidget();

    Renderer* getRenderer(void);

protected:
	void paintGL();
    void initializeGL();
    void resizeGL(int width, int height);
    Renderer *r;
};

#endif // GRAPHICS_WIDGET_H

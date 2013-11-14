#include "OpenGLWindow.hpp"
#include <QDebug>
#include <QStyle>

OpenGLWindow::OpenGLWindow(QOpenGLContext* context, QScreen* screen)
    :QWindow(screen), context(context)
{
    setSurfaceType(OpenGLSurface);
    setFormat(context->format());
    create();

    context->makeCurrent(this);

    resize(1280,800);



    renderer = new Renderer();
    camera = renderer->camera;

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGl()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGl()));
}

void OpenGLWindow::initialize()
{
    context->makeCurrent(this);
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 1.0f, 0);

    int errorCode = glGetError();
    if(errorCode != 0){
        printf("error after initing gl is: %x \n",errorCode);
    }
}

void OpenGLWindow::registerEventManager(Camera *_camera)
{
    camera = _camera;
}

void OpenGLWindow::update()
{
    paintGl();
}

void OpenGLWindow::resizeGl()
{
    context->makeCurrent(this);

    renderer->pMatrix.setToIdentity();
    renderer->pMatrix.perspective(60.0, (float) width() / (float) height(), 0.1, 1000);

    glViewport(0, 0, width(), height());
}

void OpenGLWindow::paintGl()
{
    context->makeCurrent(this);
    renderer->repaint();
    context->swapBuffers(this);
}

Renderer *OpenGLWindow::getRenderer() const
{
    return renderer;
}

void OpenGLWindow::setRenderer(Renderer *value)
{
    renderer = value;
}

void OpenGLWindow::keyPressEvent(QKeyEvent *e)
{
    if(camera == NULL){
        qDebug() << "No event manager is registerd";
        return ;
    }

    //qDebug() << "KeyPress: " << e->text();
    switch(e->key()){

    case Qt::Key_Escape:
        close();
        break;
    default:
        camera->keyPressEvent(e);
        break;
    }
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *e)
{
    //qDebug() << "KeyRelease: " << e->text();
    switch(e->key()){
    default:
        camera->keyReleaseEvent(e);
        break;
    }
}

#include "OpenGLWindow.hpp"
#include <QDebug>
#include <QStyle>
#include <QToolTip>
#include <QKeyEvent>
#include <QCursor>

#include <iostream>
OpenGLWindow::OpenGLWindow(QOpenGLContext* context, QScreen* screen)
    :QWindow(screen), context(context)
{
    setSurfaceType(OpenGLSurface);
    setFormat(context->format());
    std::cerr << "OpenGL version: "+std::to_string(context->format().majorVersion())+"."+std::to_string(context->format().minorVersion()) << std::endl;
    create();

    context->makeCurrent(this);

    resize(1280,800);

    renderer = new Renderer();
    renderer->setSize(width(), height());
    camera = renderer->camera;
    camera->setPosition(QVector3D(50,20,0));
    camera->setLookAtDirection(QVector3D(0,0,1));

    trackMouse = false;

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGl()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGl()));

    initializeOpenGLFunctions();
}

void OpenGLWindow::initialize()
{
    context->makeCurrent(this);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    renderer->setSize(width(), height());
    renderer->pMatrix.setToIdentity();
    float FOVvert = 60.0;
    float aspectRatio = (float) width() / (float) height();
    float FOVhorz = FOVvert*aspectRatio;
    float nearPlane = 0.1;
    float farPlane = 1000;
    renderer->pMatrix.perspective(FOVvert, aspectRatio, nearPlane, farPlane);

    //Near
    float nearVert = nearPlane*atanf(FOVvert/2);
    float nearHorz = nearPlane*atanf(FOVhorz/2);
    qDebug() << "nearVert" << nearVert;
    qDebug() << "nearHorz" << nearHorz;
    float NLL;
    float NLR;
    float NUL;
    float NUR;

    //Far
    float FLL;
    float FLR;
    float FUL;
    float FUR;


    renderer->pMatrixInv = renderer->pMatrix;
    renderer->pMatrixInv.inverted();
    camera->pMatrix = renderer->pMatrix;
    camera->pMatrixInv = renderer->pMatrixInv;

    glViewport(0, 0, width(), height());
}

void OpenGLWindow::paintGl()
{
    context->makeCurrent(this);
    renderer->repaint();
    context->swapBuffers(this);
}

void OpenGLWindow::setRenderer(Renderer *value)
{
    renderer = value;
}

Renderer* OpenGLWindow::getRenderer()
{
    return renderer;
}

void OpenGLWindow::setHostApplication(QApplication* _application)
{
    application = _application;
}

void OpenGLWindow::keyPressEvent(QKeyEvent *e)
{
    if(camera == NULL){
        qDebug() << "No event manager is registerd";
        return ;
    }

    switch(e->key()){

    case Qt::Key_Escape:
        close();
        break;
    case Qt::Key_T:
        specialValue += 0.01;
        std::cerr << specialValue;
        break;
    case Qt::Key_R:
        trackMouse = !trackMouse;
        if(trackMouse){
            application->setOverrideCursor(QCursor(Qt::BlankCursor));
            QPoint mousePosition = QPoint(width()/2, height()/2);
            QCursor::setPos(mapToGlobal(mousePosition));
        } else {
            application->setOverrideCursor(QCursor(Qt::ArrowCursor));
        }

        break;
    default:
        camera->keyPressEvent(e);
        break;
    }
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent *e)
{
    switch(e->key()){
    default:
        camera->keyReleaseEvent(e);
        break;
    }
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent *e)
{
    if(trackMouse){
        QPoint mousePosition = mapFromGlobal(QCursor::pos());
        int dX = width()/2 - mousePosition.x();
        int dY = height()/2 - mousePosition.y();
        camera->mouseMoveEvent(dX, dY);
        mousePosition = QPoint(width()/2, height()/2);
        QCursor::setPos(mapToGlobal(mousePosition));
    } else {
        /*
        QToolTip::showText(e->globalPos(),
                               QString::number( e->pos().x() ) + ", " +
                               QString::number( e->pos().y() ) );
                               */
    }
}

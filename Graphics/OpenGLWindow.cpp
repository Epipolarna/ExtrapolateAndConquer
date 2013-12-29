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

    //renderer->setSize(width(), height());
    //renderer->pMatrix.setToIdentity();

    float FOVvert = 60.0;
    float aspectRatio = (float) width() / (float) height();
    float nearPlane = 0.1;
    float farPlane = 1000;
    //renderer->pMatrix.perspective(FOVvert, aspectRatio, nearPlane, farPlane);

    //this is not ok.
    #define PI 3.14159265359

    //Near
    float nearVert = nearPlane*tanf(FOVvert/2 * PI/180);
    float nearHorz = nearVert*aspectRatio;
    //qDebug() << "nearVert" << nearVert;
    //qDebug() << "nearHorz" << nearHorz;
    QVector4D NLL(-nearHorz,-nearVert,-nearPlane,1);
    QVector4D NLR(nearHorz,-nearVert,-nearPlane,1);
    QVector4D NUL(-nearHorz,nearVert,-nearPlane,1);
    QVector4D NUR(nearHorz,nearVert,-nearPlane,1);

    //Far
    float farVert = farPlane*tanf(FOVvert/2 * PI/180);
    float farHorz = farVert*aspectRatio;
    //qDebug() << "farVert" << farVert;
    //qDebug() << "farHorz" << farHorz;
    QVector4D FLL(-farHorz,-farVert,-farPlane,1);
    QVector4D FLR(farHorz,-farVert,-farPlane,1);
    QVector4D FUL(-farHorz,farVert,-farPlane,1);
    QVector4D FUR(farHorz,farVert,-farPlane,1);

    glViewport(0, 0, width(), height());
}

void OpenGLWindow::paintGl()
{
    context->makeCurrent(this);
    renderer->draw();
    context->swapBuffers(this);
}

void OpenGLWindow::setRenderer(RenderSystem *renderer)
{
    this->renderer = renderer;
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

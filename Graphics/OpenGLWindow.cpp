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

    //resize(1280,800);
    resize(1920,1080);

    renderer = new Renderer();
    renderer->setSize(width(), height());
    camera = renderer->camera;
    camera->setPosition(QVector3D(0,0,0));
    camera->setLookAtDirection(QVector3D(0,0,1));

    trackMouse = false;

    connect(this, SIGNAL(widthChanged(int)), this, SLOT(resizeGl()));
    connect(this, SIGNAL(heightChanged(int)), this, SLOT(resizeGl()));

    requestNewWorld = false;

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
    float nearPlane = 0.1;
    float farPlane = 1000;
    renderer->pMatrix.perspective(FOVvert, aspectRatio, nearPlane, farPlane);

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

    renderer->frustumCorners.clear();
    renderer->frustumCorners.push_back(NLL);
    renderer->frustumCorners.push_back(NLR);
    renderer->frustumCorners.push_back(NUL);
    renderer->frustumCorners.push_back(NUR);
    renderer->frustumCorners.push_back(FLL);
    renderer->frustumCorners.push_back(FLR);
    renderer->frustumCorners.push_back(FUL);
    renderer->frustumCorners.push_back(FUR);
/*
    qDebug() << "NLL" << NLL;
    qDebug() << "NLR" << NLR;
    qDebug() << "NUL" << NUL;
    qDebug() << "NUR" << NUR;
    qDebug() << "FLL" << FLL;
    qDebug() << "FLR" << FLR;
    qDebug() << "FUL" << FUL;
    qDebug() << "FUR" << FUR;
*/

    renderer->pMatrixInv = renderer->pMatrix;
    renderer->pMatrixInv.inverted();

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

    case Qt::Key_1:
        renderer->shadowLevel = 1;
        break;
    case Qt::Key_2:
        renderer->shadowLevel = 2;
        break;
    case Qt::Key_3:
        renderer->shadowLevel = 3;
        break;

    case Qt::Key_Z:
        renderer->isRenderingTerrain = !renderer->isRenderingTerrain;
        break;
    case Qt::Key_X:
        renderer->isRenderingTrees = !renderer->isRenderingTrees;
        break;
    case Qt::Key_C:
        renderer->isRenderingBalls = !renderer->isRenderingBalls;
        break;
    case Qt::Key_V:
        renderer->isRenderingShadows = !renderer->isRenderingShadows;
        break;
    case Qt::Key_N:
        requestNewWorld = true;
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

bool OpenGLWindow::isNewWorldRequested() {
    bool returnValue = requestNewWorld;
    requestNewWorld = false;
    return returnValue;
}

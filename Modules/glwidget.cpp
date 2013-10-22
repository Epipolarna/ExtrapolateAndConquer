#include "glwidget.h"
#include <stdio.h>
#include <iostream>
#include <GL/gl.h>

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(), parent)
{
    phongShader = new QGLShaderProgram;
    threshShader = new QGLShaderProgram;
    texShader = new QGLShaderProgram;
    glContext = context()->contextHandle();
    fpsMeter.start();
}

GLWidget::~GLWidget()
{
}

void GLWidget::loadModel(){
}

void loadTexture(){

}

QSize GLWidget::sizeHint() const
{
    return QSize(640, 480);
}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    //FBO1 = new QGLFramebufferObject(width, height, FBO1->format());

    glViewport(0, 0, width, height);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();    // Very important line to get all OpenGL Functionality

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);

    // ---------- SHADER INIT -----------------
    initShader(phongShader, "Shaders/phong.vert", "Shaders/phong.frag");
    initShader(threshShader, "Shaders/FBO/threshold.vert", "Shaders/FBO/threshold.frag");
    initShader(texShader, "Shaders/plaintextureshader.vert", "Shaders/plaintextureshader.frag");

    // ----------- FBO ------------------------
    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
            qWarning("This system has no framebuffer object support");
    }
    // QGLFramebufferObject::Depth enables the depth buffer for the FBO
    fboFormat.setInternalTextureFormat(GL_RGBA16F);
    fboFormat.setAttachment(QGLFramebufferObject::Depth);
    fboFormat.setSamples(0);

    FBO1 = new QGLFramebufferObject(width(), height(), fboFormat);
    FBO2 = new QGLFramebufferObject(width(), height(), fboFormat);
    FBO3 = new QGLFramebufferObject(width(), height(), fboFormat);

    // ---------- MODELS -----------------------

    model = new Model("Models/monkey.obj");
    squareModel = new Model("Models/fboSquare.obj");

    object = new Object(model, phongShader);
    square = new Object(squareModel, texShader);

    player = new Player();

    // ----------- TEXTURE LOADING ------------
    tex = bindTexture(QPixmap(QString("Models/GtaRE.jpg")), GL_TEXTURE_2D);

    glClearColor(0.3f, 0.8f, 0.6f, 0);
}

void GLWidget::paintGL()
{
    // Draw scene to the FBO1
    //useFBO(FBO1);
    //glViewport(???); might be needed to support resize
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    player->setPosition(1,0,-3);
    object->draw(player, pMatrix);
    //QImage im1 = FBO1->toImage();
    //im1.save("im1.png");
/*
    useFBO(FBO2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square->program = threshShader;
    square->draw(player, pMatrix, FBO1->texture());
    //QImage im2 = FBO2->toImage();
    //im2.save("im2.png");

    // Draw the FBO to a square and display
    QGLFramebufferObject::bindDefault();    // Set the "normal" screen as render target
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square->program = texShader;
    square->draw(player, pMatrix, FBO2->texture());

    //qDebug(QByteArray::number(GL_TEXTURE0));
    //qDebug(QByteArray::number(GL_TEXTURE1));
    //qDebug(QByteArray::number(FBO1->texture()));
    //qDebug(QByteArray::number(FBO2->texture()));

    // Render text to screen
    nanoSex = fpsMeter.nsecsElapsed();
    renderText(20, 20, QString::number(nanoSex));
    fpsMeter.start();
    */
}

void GLWidget::initShader(QGLShaderProgram* shader, QString vertexPath, QString fragmentPath){
    shader->addShaderFromSourceFile(QGLShader::Vertex,  vertexPath);
    shader->addShaderFromSourceFile(QGLShader::Fragment,fragmentPath);
    shader->link();
}

void GLWidget::useFBO(QGLFramebufferObject* FBO){
    FBO->bind();
    // Since the GL_TEXTURE? cannot be found in QGLFBO it has to
    // expressed relative GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0 + FBO->texture());
    glBindTexture(GL_TEXTURE_2D, FBO->texture());
}

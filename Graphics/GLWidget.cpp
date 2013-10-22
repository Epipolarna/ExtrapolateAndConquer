#include "GLWidget.hpp"
//#include "ui_GLWidget.h"

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(QGLFormat(), parent)
{
    phongShader = new QGLShaderProgram;
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glEnable(GL_TEXTURE_2D);
    glClearColor(0.3f, 0.8f, 0.6f, 0);

    // ---------- SHADER INIT -----------------
    initShader(phongShader, "Graphics/Shaders/phong.vert", "Graphics/Shaders/phong.frag");
    //initShader(threshShader, "Shaders/FBO/threshold.vert", "Shaders/FBO/threshold.frag");
    //initShader(texShader, "Shaders/plaintextureshader.vert", "Shaders/plaintextureshader.frag");

    // ----------- FBO ------------------------
    /*
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
    */

    // ---------- MODELS -----------------------
    monkeyModel = new graphics::Model("Graphics/Models/monkey.obj");

    //squareModel = new Model("Models/fboSquare.obj");

    //object = new Object(model, phongShader);
    //square = new Object(squareModel, texShader);

    //player = new Player();

    // ---------- OBJECTS -----------------------

    monkey = new graphics::Object(monkeyModel, phongShader);


    // ---------- CAMERAS -----------------------

    player = new graphics::Camera();
    player->setPosition(QVector3D(3,0,-3));
    player->setLookAt(QVector3D(0,0,0));
    player->setUp(QVector3D(0,1,0));


    // ----------- TEXTURE LOADING ------------
    //tex = bindTexture(QPixmap(QString("Models/GtaRE.jpg")), GL_TEXTURE_2D);
}

void GLWidget::paintGL()
{
    fpsMeter.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    monkey->draw(player->vMatrix, pMatrix);

    // Render text to screen
    nanoSex = fpsMeter.nsecsElapsed();
    renderText(20, 20, "FPS: " + QString::number(1.0e9/nanoSex));
}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.001, 1000);

    glViewport(0, 0, width, height);
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

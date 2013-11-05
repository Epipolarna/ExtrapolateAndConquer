#include "GLWidget.hpp"
#include "TerrainGenerator.hpp"

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :
    QGLWidget(format, parent)
{
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    timer->start(16);
}

GLWidget::~GLWidget()
{
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.3f, 0.8f, 0.6f, 0);

    QGLFormat f = format();
    qDebug() << "GL version: " << f.majorVersion() << f.minorVersion();

    QString resourcePath = "./resources/";
    QString modelPath = resourcePath + "models/";
    QString shaderPath = resourcePath + "shaders/";
    QString texturePath = resourcePath + "textures/";


    // ---------- SHADER INIT -----------------
    skyboxShader = initShader(shaderPath+"skyboxShader.vert", shaderPath+"skyboxShader.frag");
    phongShader = initShader(shaderPath+"phong.vert", shaderPath+"phong.frag");
    phongTexShader = initShader(shaderPath+"phongTex.vert", shaderPath+"phongTex.frag");
    flatShader = initShader(shaderPath+"flat.vert", shaderPath+"flat.frag");

    // ---------- MODELS -----------------------
    skyboxModel = new graphics::Model(modelPath+"skybox.obj");
    oceanModel = new graphics::Model(modelPath+"unitSquare.obj");
    monkeyModel = new graphics::Model(modelPath+"monkey.obj");

    ml.loadModel(modelPath+"skybox.obj");

    simplexModel = TerrainGenerator::simplexTerrain(100,100, 10,10, 5);

    // ---------- TEXTURE LOADING --------------
    skyboxTex = uploadTexture(texturePath+"skybox0.png", false);
    oceanTex = uploadTexture(texturePath+"water.png", true);

    // ---------- OBJECTS -----------------------
    skybox = new graphics::Object(skyboxModel, skyboxShader, skyboxTex);
    ocean = new graphics::Object(oceanModel, phongTexShader, oceanTex);
    //ocean->setColor(59,58,99,255);
    ocean->setScale(1000,1,1000);
    monkey = new graphics::Object(monkeyModel, phongShader);

    simplex = new graphics::Object(simplexModel, flatShader);
    simplex->setColor(85,196,48,255);
    simplex->setScale(10,1,10);
    simplex->setPosition(-500,0,-500);

    // ---------- CAMERAS -----------------------
    player = new graphics::Camera();

    currentCamera = player;
}

void GLWidget::paintGL()
{
    fpsMeter.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    skybox->draw(currentCamera->skyboxMatrix(), pMatrix);
    glEnable(GL_DEPTH_TEST);

    ocean->draw(currentCamera->vMatrix, pMatrix);
    simplex->draw(currentCamera->vMatrix, pMatrix);
    glEnable(GL_CULL_FACE);

    //monkey->draw(currentCamera->vMatrix, pMatrix);

    // Render text to screen
    nanoSex = fpsMeter.nsecsElapsed();
    //renderText(20, 20, "FPS: " + QString::number(1.0e9/nanoSex));
}

void GLWidget::resizeGL(int width, int height)
{
    if (height == 0) {
        height = 1;
    }

    pMatrix.setToIdentity();
    pMatrix.perspective(60.0, (float) width / (float) height, 0.1, 1000);

    glViewport(0, 0, width, height);
}

QGLShaderProgram* GLWidget::initShader(QString vertexPath, QString fragmentPath){
    QGLShaderProgram* shader = new QGLShaderProgram;
    shader->addShaderFromSourceFile(QGLShader::Vertex,  vertexPath);
    shader->addShaderFromSourceFile(QGLShader::Fragment,fragmentPath);
    shader->link();
    return shader;
}

GLuint GLWidget::uploadTexture(QString imagePath, bool mipmap)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    QImage image(imagePath);
    QImage imageGL = convertToGLFormat(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageGL.width(), imageGL.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageGL.bits());

    if(mipmap){
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    return textureID;
}

void GLWidget::useFBO(QGLFramebufferObject* FBO){
    FBO->bind();
    // Since the GL_TEXTURE? cannot be found in QGLFBO it has to
    // expressed relative GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0 + FBO->texture());
    glBindTexture(GL_TEXTURE_2D, FBO->texture());
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "GLWidget KeyPress: " << e->text();
    switch(e->key()){

    case Qt::Key_Escape:
        close();
        break;

    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:

    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Down:
    case Qt::Key_Right:

    case Qt::Key_Q:
    case Qt::Key_E:

    case Qt::Key_Space:
    case Qt::Key_Control:
        currentCamera->keyPressEvent(e);
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *e)
{
    qDebug() << "GLWidget KeyRelease: " << e->text();
    switch(e->key()){
    case Qt::Key_W:
    case Qt::Key_A:
    case Qt::Key_S:
    case Qt::Key_D:

    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_Down:
    case Qt::Key_Right:

    case Qt::Key_Q:
    case Qt::Key_E:

    case Qt::Key_Space:
    case Qt::Key_Control:
        currentCamera->keyReleaseEvent(e);
        break;
    default:
        QWidget::keyReleaseEvent(e);
        break;
    }
}

void GLWidget::closeEvent(QCloseEvent *e)
{
    e->accept();
}

void GLWidget::timerUpdate()
{
    player->updatePosition();
    updateGL();
}

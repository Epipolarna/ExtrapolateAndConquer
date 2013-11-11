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
    glClearColor(0.0f, 0.0f, 1.0f, 0);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    QGLFormat f = format();
    qDebug() << "GL version: " << f.majorVersion() << f.minorVersion();

    QString resourcePath = "./resources/";
    QString modelPath = resourcePath + "models/";
    QString shaderPath = resourcePath + "shaders/";
    QString texturePath = resourcePath + "textures/";


    // ---------- SHADER INIT -----------------
    skyboxShader = initShader(shaderPath+"skyboxShader.vert", shaderPath+"skyboxShader.frag");
    oceanShader = initShader(shaderPath+"oceanShader.vert", shaderPath+"oceanShader.frag");
    terrainShader = initShader(shaderPath+"terrainShader.vert", shaderPath+"terrainShader.frag");
    phongShader = initShader(shaderPath+"phong.vert", shaderPath+"phong.frag");
    phongTexShader = initShader(shaderPath+"phongTex.vert", shaderPath+"phongTex.frag");
    flatShader = initShader(shaderPath+"flat.vert", shaderPath+"flat.frag");

    // ---------- MODELS -----------------------
    skyboxModel = new graphics::Model(modelPath+"skybox.obj");
    oceanModel = new graphics::Model(modelPath+"unitSquare.obj");
    humanModel = new graphics::Model(modelPath+"gordon.obj");
    tree0Model = new graphics::Model(modelPath+"tree0.obj");
    //tree1Model = new graphics::Model(modelPath+"tree1.obj");
    roverModel = new graphics::Model(modelPath+"rover.obj");
    monkeyModel = new graphics::Model(modelPath+"monkey.obj");

    altSkybox = new ModelLoader();
    altSkybox->loadModel(modelPath+"skybox.obj");
    altMonkey = new ModelLoader();
    altMonkey->loadModel(modelPath+"teapot.obj");

    //terrainModel = TerrainGenerator::simplexTerrain(100,100, 10,10, 5);
    //float octaves[] = {400, 200, 100, 50, 20, 10};
    //float scales[] =  {20,  5,   4,   3,  1.5,  1};

    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/1.87;
    float gain = 0.60;

    //for each pixel, get the value
    float period = 400;
    float amplitude = 20;
    for (int i = 0; i < 16; i++)
    {
        octaves[i] = period;
        scales[i] = amplitude;

        period *= lacunarity;
        amplitude *= gain;
    }

    int nOctaves = sizeof(octaves)/sizeof(float);
    terrainModel = TerrainGenerator::simplexTerrain2(1000,1000,0.5f,octaves,scales,nOctaves);

    // ---------- TEXTURE LOADING --------------
    skyboxTex = uploadTexture(texturePath+"skybox0.png", false);
    oceanTex = uploadTexture(texturePath+"water.png", true);
    oceanNormalMap = uploadTexture(texturePath+"waterNormalMap2.png", true);
    grassTex = uploadTexture(texturePath+"grass1.png", true);
    //grassNormalMap = uploadTexture(texturePath+"grassNormalMap0.png", true);
    rockTex = uploadTexture(texturePath+"rock0.png", true);

    // ---------- OBJECTS -----------------------
    skybox = new graphics::Object(altSkybox, skyboxShader, skyboxTex);

    ocean = new graphics::Object(oceanModel, oceanShader, oceanTex, grassTex, oceanNormalMap);
    ocean->setShadingParameters(0.1, 0.6, 3.0, 50);
    ocean->setColor(59,58,99,200);
    ocean->setScale(1000,1,1000);
    ocean->setTexScaling(100);

    terrain = new graphics::Object(terrainModel, terrainShader, grassTex, 0, 0);
    terrain->setShadingParameters(0.3, 0.7, 0.3, 50);
    terrain->setColor(85,196,48,255);
    terrain->setPosition(-500,0,-500);
    terrain->setTexScaling(1000);

    human = new graphics::Object(humanModel, phongShader);
    human->setPosition(-200, 0, -200);
    human->setColor(204,69,38,255);

    tree0 = new graphics::Object(tree0Model, phongShader);
    tree0->setPosition(-195, 0, -200);
    tree0->setColor(131,165,255,255);

    rover = new graphics::Object(roverModel, phongShader);
    rover->setPosition(-195, 0, -195);
    rover->setColor(182,255,73,255);

    monkey = new graphics::Object(altMonkey, phongShader);
    monkey->setScale(0.1,0.1,0.1);

    // ---------- CAMERAS -----------------------
    player = new graphics::Camera();
    player->setPosition(QVector3D(-210,2,-210));

    currentCamera = player;

    int errorCode = glGetError();
    if(errorCode != 0){
        printf("error after initing gl is: %x \n",errorCode);
    }
}

void GLWidget::paintGL()
{
    fpsMeter.start();

    int errorCode = glGetError();
    if(errorCode != 0){
        printf("glerror at redraw is: 0x%x",errorCode);
        exit(0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    skybox->draw2(currentCamera->skyboxMatrix(), pMatrix);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    terrain->draw(currentCamera->vMatrix, pMatrix);

    human->draw(currentCamera->vMatrix, pMatrix);
    tree0->draw(currentCamera->vMatrix, pMatrix);
    rover->draw(currentCamera->vMatrix, pMatrix);

    // Should be drawn last to favor transparency
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    ocean->draw(currentCamera->vMatrix, pMatrix);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
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

QOpenGLShaderProgram* GLWidget::initShader(QString vertexPath, QString fragmentPath){
    QOpenGLShaderProgram* shader = new QOpenGLShaderProgram;
    shader->addShaderFromSourceFile(QOpenGLShader::Vertex,  vertexPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath);
    shader->link();
    return shader;
}

GLuint GLWidget::uploadTexture(QString imagePath, bool mipmap)
{

    int errorCode = glGetError();
    if(errorCode != 0){
        printf("glError on attempting to upload texture: 0x%x \n",errorCode);
        printf("broken texture was: %s \n",imagePath.toStdString().c_str());
        exit(0);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    QImage image(imagePath);
    QImage imageGL = convertToGLFormat(image);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageGL.width(), imageGL.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageGL.bits());
    
    if(mipmap){
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    errorCode = glGetError();
    if(errorCode != 0){
        printf("glError on texture upload was: 0x%x \n",errorCode);
        printf("broken texture was: %s \n",imagePath.toStdString().c_str());
        exit(0);
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

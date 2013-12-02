#include "ExtrapolateAndConquer.hpp"
#include <QDesktopWidget>
#include <QStyle>

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[]){

    application = new QApplication(argc,argv);

    QSurfaceFormat format;
    format.setVersion(3,2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);

    context = new QOpenGLContext;
    context->setFormat(format);
    context->create();

    openGLWindow = new OpenGLWindow(context);
    openGLWindow->setHostApplication(application);
    openGLWindow->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                                  Qt::AlignCenter,
                                                  openGLWindow->size(),
                                                  application->desktop()->availableGeometry()));
    openGLWindow->show();

    camera = openGLWindow->getRenderer()->camera;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));
    fpsMeter = new QElapsedTimer;
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

void ExtrapolateAndConquer::initialize(void){

    openGLWindow->initialize();

    resourceManager = new ResourceManager;
    loadResources();

    Renderer* renderer = openGLWindow->getRenderer();

    Object* o1 = new Object(resourceManager->getModel("tree0"),resourceManager->getShader("phong"));
    //renderer->renderList.push_back(o1);

    //the skybox needs to be specially added to the renderer
    Object* skybox = new Object(resourceManager->getModel("skybox"),resourceManager->getShader("skyboxShader"),resourceManager->getTexture("skybox1"));
    renderer->skybox = skybox;

    // FBO setup
    renderer->fboSquare = new Object(resourceManager->getModel("fboSquare"),resourceManager->getShader("fbo"),renderer->fbo1->colorTex);
    renderer->depthProgram = resourceManager->getShader("depth");


    int nBalls = 100;
    printf("initing %d balls \n ",nBalls);
    for(int i = 0; i < nBalls; i++){
        // Initialize entity
        e = &entityManager.createEntity();

        // Add Sphere physics
        e->add<SpherePhysics>();
        SpherePhysics & sp = e->get<SpherePhysics>();
        sp.position = QVector3D(qrand()%100+100,5,qrand()%100+100);
        sp.rotation2 = QQuaternion(1,0,0,0);
        sp.mass = 100.0;
        sp.elasticity = 0.3;
        sp.friction = 1.0;
        sp.radius = 1.0;
        sp.gravitationalConstant = 9.82;
        sp.momentOfInertia = 6.0/12.0 * sp.mass * sp.radius * sp.radius;

        // Add Graphics
        e->add<Graphics>();
        e->get<Graphics>().object = new Object(resourceManager->getModel("unitSphere10"), resourceManager->getShader("phongTex"), resourceManager->getTexture("sphere"));
        e->get<Graphics>().object->setScale(sp.radius);
        e->get<Graphics>().object->setShaderParameters(0.3, 0.5, 0.2, 100);

        renderer->drawObject(e->get<Graphics>().object);
    }

    // ------------- Generate world ------------------------
    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/1.87;
    float gain = 0.66;

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

    Model* worldModel;
    world = new World();
    renderer->lightPosition = world->lightPosition;
    QMatrix4x4 mat;
    mat.setToIdentity();
    mat.lookAt(renderer->lightPosition, QVector3D(150,-100,150), QVector3D(0,1,0));
    renderer->lightSourceVMatrix = mat;
    Object* worldObject;

    int nOctaves = sizeof(octaves)/sizeof(float);
    worldModel = world->generateWorld(300,300,0.5f,octaves,scales,nOctaves);
    hightMapOfChunk = world->heightMap;

    sphereTerrainCollisionSystem.setHeightMap((hightMapOfChunk*2*world->scaleFactor-world->scaleFactor));
    sphereTerrainCollisionSystem.setWorld(world);

    QVector<GLuint> worldTextures = QVector<GLuint>();
    worldTextures.push_back(resourceManager->getTexture("TerrainAtlasDiffuse1"));
    worldTextures.push_back(0);
    worldTextures.push_back(0);
    worldTextures.push_back(renderer->fbo1->depthTex);

    worldObject = new Object(worldModel, resourceManager->getShader("terrainAtlasShader"), worldTextures);
    worldObject->setShaderParameters(0.7f, 0.5f, 0.5f, 20);
    worldObject->setColor(85,196,48,255);
    //worldObject->setScale(2,0,2);
    worldObject->setTexScaling(1000);

    renderer->world = worldObject;

    QVector<GLuint> ot = QVector<GLuint>();
    ot.push_back(resourceManager->getTexture("water"));
    ot.push_back(resourceManager->getTexture("skyboxWaterReflection"));
    ot.push_back(resourceManager->getTexture("waterNormalMap2"));
    ot.push_back(renderer->fbo1->depthTex);

    Object* ocean = new Object(resourceManager->getModel("hiResSquare"), resourceManager->getShader("oceanShader"),ot);
    
    ocean->setShaderParameters(0.4f, 0.4f, 1.0f, 50);
    ocean->setColor(59,58,99,200);
    //ocean->setScale(500,1,500);
    ocean->setTexScaling(2000);

    renderer->water = ocean;

    //tree stuff..
    renderer->treeShader = resourceManager->getShader("instance");
    renderer->treeModel = resourceManager->getModel("tree0");
    renderer->treeTexture = resourceManager->getTexture("sphere");
    renderer->treePositions = world->getTrees();

    printf("placed %lu trees \n",renderer->treePositions.size());

    // Global influence map
    int resolution = 4;
    cv::Mat influenceMap = cv::Mat::zeros(world->sizeX*resolution, world->sizeZ*resolution, CV_8U);

    for(SpherePhysics & physics : entityManager.getComponents<SpherePhysics>()) {
        cv::circle(influenceMap, cv::Point2i(physics.position.x()*resolution, physics.position.z()*resolution), physics.radius, cv::Scalar(255), -1);
    }
    cv::dilate(influenceMap, influenceMap, cv::Mat::ones(3,3,CV_8U), cv::Point2i(-1,-1), 3);
    cv::GaussianBlur(influenceMap, influenceMap, cv::Size(9,9), 3);


    // Initialize systems
    graphicsUpdateSystem.initialize(entityManager);
    spherePhysicsSystem.initialize(entityManager);
    spherePhysicsSystem.setTimeInterval(0.01);  // Set dt. QTimer::interval() is in milliseconds
    sphereSphereCollisionSystem.initialize(entityManager);
    sphereTerrainCollisionSystem.initialize(entityManager);
    aiSystem.initialize(entityManager);
    aiSystem.setInfluenceMap(influenceMap);
    
    printf("all initialization done! \n");
}

void ExtrapolateAndConquer::loadResources(void){

    //FBO Square. Used to draw the scene on when it has been drawn to a FBO
    printf("loading shadowMapping data \n");
    resourceManager->loadShader("fbo");
    resourceManager->loadShader("depth");
    resourceManager->loadModel("fboSquare");

    //test data
    printf("loading teapot data \n");
    resourceManager->loadShader("phong");
    resourceManager->loadModel("teapot");
    resourceManager->loadModel("tree0");

    printf("loading sphere data \n");
    resourceManager->loadShader("phongTex");
    resourceManager->loadModel("unitSphere10");
    resourceManager->loadTexture("sphere");

    //skybox data
    printf("loading skybox data \n");
    resourceManager->loadModel("skybox");
    resourceManager->loadTexture("skybox1");
    resourceManager->loadShader("skyboxShader");

    //ground data
    printf("loading ground data \n");
    resourceManager->loadShader("terrainShader");
    resourceManager->loadTexture("grass1", true);
    //ground data using texture atlas
    resourceManager->loadShader("terrainAtlasShader");
    resourceManager->loadTextureAtlas("TerrainAtlasDiffuse1", 2, true);

    //water data
    printf("loading water data \n");
    //resourceManager->loadTexture("water", true);
    resourceManager->loadTexture("waterNormalMap2", true);
    //resourceManager->loadTexture("waterNormalMap1");
    //resourceManager->loadModel("unitSquare");
    resourceManager->loadModel("hiResSquare");
    resourceManager->loadShader("oceanShader");

    resourceManager->loadTexture("skyTop", true);
    resourceManager->loadTexture("skyboxWaterReflection", true);

    resourceManager->loadShader("instance");

    printf("all resources loaded! \n");
}

int ExtrapolateAndConquer::run(){

    initialize();
    timer->start(2);
    int returnCode = application->exec();
    return returnCode;
}

bool first = true;
void ExtrapolateAndConquer::loopBody(){
    timer->stop();
    camera->updatePosition();
    world->getHeight(camera->position.x(), camera->position.z());
    world->getNormal(camera->position.x(), camera->position.z());

    spherePhysicsSystem.setTimeInterval(fpsMeter->elapsed()/1000.0);  // Set dt.

    SpherePhysics & sp = e->get<SpherePhysics>();
    sp.force += QVector3D(0.1,0,0.2);

    // Run collision detection
    sphereSphereCollisionSystem.batch();    // Fetches all entities containing "Collision" components
    sphereTerrainCollisionSystem.batch();

    // Run physics simulators
    spherePhysicsSystem.batch();

    // Run physics to graphics transfer of position/rotation
    graphicsUpdateSystem.batch();

    // Run AI
    aiSystem.batch();

    //make sure to update the gl widget...
    //graphicsWindow->centralWidget()->update();
    openGLWindow->update();

    elapsedTime = fpsMeter->elapsed();
    fpsMeter->restart();
    //qDebug() << "FPS: " << 1000/elapsedTime;
    timer->start();
}

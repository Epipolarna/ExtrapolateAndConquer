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
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

void ExtrapolateAndConquer::initialize(void){

    openGLWindow->initialize();

    resourceManager = new ResourceManager;
    loadResources();

    Renderer* renderer = openGLWindow->getRenderer();

    Object* o1 = new Object(resourceManager->getModel("teapot"),resourceManager->getShader("phong"));
    //renderer->renderList.push_back(o1);

    //the skybox needs to be specially added to the renderer
    Object* skybox = new Object(resourceManager->getModel("skybox"),resourceManager->getShader("skyboxShader"),resourceManager->getTexture("skybox0"));
    renderer->skybox = skybox;


    // Initialize systems
    graphicsUpdateSystem.initialize(entityManager);
    spherePhysicsSystem.initialize(entityManager);
    spherePhysicsSystem.setTimeInterval(0.01);  // Set dt. QTimer::interval() is in milliseconds
    sphereSphereCollisionSystem.initialize(entityManager);
    sphereTerrainCollisionSystem.initialize(entityManager);

    // Initialize entity
    e = &entityManager.createEntity();

    // Add Sphere physics
    e->add<SpherePhysics>();
    SpherePhysics & sp = e->get<SpherePhysics>();
    sp.position = QVector3D(10,23,10);
    sp.rotation2 = QQuaternion(1,0,0,0);
    sp.mass = 1.0;
    sp.elasticity = 0.8;
    sp.friction = 1.0;
    sp.radius = 1.0;
    sp.gravitationalConstant = 9.82;
    sp.momentOfInertia = 6.0/12.0 * sp.mass * sp.radius * sp.radius;

    // Add Graphics
    e->add<Graphics>();
    e->get<Graphics>().object = new Object(resourceManager->getModel("sphere"), resourceManager->getShader("phongTex"), resourceManager->getTexture("sphere"));
    e->get<Graphics>().object->setScale(sp.radius);

    renderer->drawObject(e->get<Graphics>().object);

    e = &entityManager.createEntity();
    // Add Sphere physics
    e->add<SpherePhysics>();
    SpherePhysics & sp2 = e->get<SpherePhysics>();
    sp2.position = QVector3D(10,30,10);
    sp2.rotation2 = QQuaternion(1,0,0,0);
    sp2.mass = 1.0;
    sp2.elasticity = 0.5;
    sp2.friction = 1.0;
    sp2.radius = 1.0;
    sp2.gravitationalConstant = 9.82;
    sp2.momentOfInertia = 6.0/12.0 * sp2.mass * sp2.radius * sp2.radius;
    // Add Graphics
    e->add<Graphics>();
    e->get<Graphics>().object = new Object(resourceManager->getModel("sphere"), resourceManager->getShader("phongTex"), resourceManager->getTexture("sphere"));
    e->get<Graphics>().object->setScale(sp2.radius);
    renderer->drawObject(e->get<Graphics>().object);


    // Generate world
    // ---------------------------
    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/1.87;
    float gain = 0.6;

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
    Object* worldObject;

    int nOctaves = sizeof(octaves)/sizeof(float);
    worldModel = world->generateWorld(100,100,1.0f,octaves,scales,nOctaves);
    hightMapOfChunk = world->heightMap;

    sphereTerrainCollisionSystem.setHeightMap((hightMapOfChunk*2*world->scaleFactor-world->scaleFactor));
    sphereTerrainCollisionSystem.setWorld(world);

    QVector<GLuint> worldTextures = QVector<GLuint>();
    worldTextures.push_back(resourceManager->getTexture("grass1"));

    worldObject = new Object(worldModel, resourceManager->getShader("terrainShader"), worldTextures);
    worldObject->setShaderParameters(0.7f, 0.5f, 0.5f, 20);
    worldObject->setColor(85,196,48,255);
    worldObject->setTexScaling(1000);

    renderer->world = worldObject;

    QVector<GLuint> ot = QVector<GLuint>();
    ot.push_back(resourceManager->getTexture("water"));
    ot.push_back(0);
    ot.push_back(resourceManager->getTexture("waterNormalMap2"));

    Object* ocean = new Object(resourceManager->getModel("unitSquare"), resourceManager->getShader("oceanShader"),ot);
    
    ocean->setShaderParameters(0.4f, 0.6f, 1.0f, 50);
    ocean->setColor(59,58,99,200);
    ocean->setScale(100,1,100);
    ocean->setTexScaling(100);

    renderer->water = ocean;
}

void ExtrapolateAndConquer::loadResources(void){

    //test data
    printf("loading teapot data \n");
    resourceManager->loadShader("phong");
    resourceManager->loadModel("teapot");

    printf("loading sphere data \n");
    resourceManager->loadShader("phongTex");
    resourceManager->loadModel("sphere");
    resourceManager->loadTexture("sphere");

    //skybox data
    printf("loading skybox data \n");
    resourceManager->loadModel("skybox");
    resourceManager->loadTexture("skybox0");
    resourceManager->loadShader("skyboxShader");

    //ground data
    printf("loading ground data \n");
    resourceManager->loadTexture("grass1", true);
    resourceManager->loadShader("terrainShader");

    //water data
    printf("loading water data \n");
    //resourceManager->loadTexture("water", true);
    resourceManager->loadTexture("waterNormalMap2", true);
    //resourceManager->loadTexture("waterNormalMap1");
    resourceManager->loadModel("unitSquare");
    resourceManager->loadShader("oceanShader");
}

int ExtrapolateAndConquer::run(){

    initialize();
    timer->start(20);
    int returnCode = application->exec();
    return returnCode;
}

bool first = true;
void ExtrapolateAndConquer::loopBody(){
    camera->updatePosition();
    world->getHeight(camera->position.x(), camera->position.z());
    world->getNormal(camera->position.x(), camera->position.z());

    SpherePhysics & sp = e->get<SpherePhysics>();
    sp.force += QVector3D(0.1,0,0.2);

    // Run collision detection
    sphereSphereCollisionSystem.batch();    // Fetches all entities containing "Collision" components
    sphereTerrainCollisionSystem.batch();

    // Run physics simulators
    spherePhysicsSystem.batch();

    // Run physics to graphics transfer of position/rotation
    graphicsUpdateSystem.batch();

    //make sure to update the gl widget...
    //graphicsWindow->centralWidget()->update();
    openGLWindow->update();
}

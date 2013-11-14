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

Entity<Components> * entity;
void ExtrapolateAndConquer::initialize(void){

    openGLWindow->initialize();

    resourceManager = new ResourceManager;
    loadResources();

    //Renderer* renderer = graphicsWindow->getRenderer();
    Renderer* renderer = openGLWindow->getRenderer();

    Object* o1 = new Object(resourceManager->getModel("teapot"),resourceManager->getShader("phong"));

    //the skybox needs to be specially added to the renderer
    Object* skybox = new Object(resourceManager->getModel("skybox"),resourceManager->getShader("skyboxShader"),resourceManager->getTexture("skybox0"));
    renderer->skybox = skybox;
    
    renderer->renderList.push_back(o1);

    // Initialize systems
    simplePhysicsSystem.initialize(entityManager);
    graphicsUpdateSystem.initialize(entityManager);
    spherePhysicsSystem.initialize(entityManager);
    spherePhysicsSystem.gravitationalConstant = 9.82;
    spherePhysicsSystem.setTimeInterval(timer->interval()/1000.0);  // Set dt. QTimer::interval() is in milliseconds
    sphereCollisionSystem.initialize(entityManager);
	
	// Initialize entity
    entity = &entityManager.createEntity();
    entity->add<SimplePhysics>();
    entity->get<SimplePhysics>().position = QVector3D(0,0,0);
    entity->get<SimplePhysics>().velocity = QVector3D(0,-0.01,0);
    entity->add<Graphics>();
    entity->get<Graphics>().object = new Object(resourceManager->getModel("teapot"), resourceManager->getShader("phong"));

    entity->add<SpherePhysics>();
    SpherePhysics & sp = entity->get<SpherePhysics>();
    sp.mass = 1.0;
    sp.elasticity = 0.1;
    sp.friction = 0.1;
    sp.radius = 1.0;
    sp.momentOfInertia = 6.0/12.0 * sp.mass * sp.radius * sp.radius;

    renderer->drawObject(entity->get<Graphics>().object);
    //printf("Pointer to object is: %x \n",entity->get<Graphics>().object);


    // Generate world
    // ---------------------------
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

    Model* world;
    WorldGenerator wg = WorldGenerator();
    Object* worldObject;

    int nOctaves = sizeof(octaves)/sizeof(float);
    world = wg.generateWorld(1000,1000,0.5f,octaves,scales,nOctaves);

    QVector<GLuint> gt = QVector<GLuint>();
    gt.push_back(resourceManager->getTexture("grass"));
    gt.push_back(resourceManager->getTexture("grass"));
    gt.push_back(resourceManager->getTexture("grass"));

    worldObject = new Object(world, resourceManager->getShader("terrainShader"), gt);
    worldObject->setShaderParameters(0.3, 0.7, 0.3, 50);
    worldObject->setColor(85,196,48,255);
    worldObject->setPosition(-500,0,-500);
    //worldObject->setTexScaling(1000);

    renderer->world = worldObject;

    QVector<GLuint> ot = QVector<GLuint>();
    ot.push_back(resourceManager->getTexture("water"));
    ot.push_back(0);
    ot.push_back(resourceManager->getTexture("waterNormalMap2"));

    Object* ocean = new Object(resourceManager->getModel("unitSquare"), resourceManager->getShader("oceanShader"),ot);
    
    ocean->setShaderParameters(0.1, 0.6, 3.0, 50);
    ocean->setColor(59,58,99,200);
    ocean->setScale(1000,1,1000);
    ocean->setTexScaling(100);

    renderer->water = ocean;
}

void ExtrapolateAndConquer::loadResources(void){
    
    //test data
    printf("loading teapot data \n");
    resourceManager->loadShader("phong");
    resourceManager->loadModel("teapot");

    //skybox data
    printf("loading skybox data \n");
    resourceManager->loadModel("skybox");
    resourceManager->loadTexture("skybox0");
    resourceManager->loadShader("skyboxShader");

    //ground data
    printf("loading ground data \n");
    resourceManager->loadTexture("grass");
    resourceManager->loadShader("terrainShader");

    //water data
    printf("loading water data \n");
    resourceManager->loadTexture("water", true);
    resourceManager->loadTexture("waterNormalMap2");
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


    // Run the systems...
    simplePhysicsSystem.batch();
    graphicsUpdateSystem.batch();

    // Run collision detection
    //sphereCollisionSystem.batch();    // Fetches all entities containing "Collision" components

    // Run physics simulators
    //simplePhysicsSystem.batch();
    //spherePhysicsSystem.batch();

    // Run physics to graphics transfer of position/rotation
    //graphicsUpdateSystem.batch();
    //


    //make sure to update the gl widget...
    //graphicsWindow->centralWidget()->update();
    openGLWindow->update();
}

#include "ExtrapolateAndConquer.hpp"

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[]){
    
    application = new QApplication(argc,argv);
    graphicsWindow = new GraphicsWindow();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));

    cam = graphicsWindow->getRenderer()->cam;

    graphicsWindow->registerEventManager(cam);
    graphicsWindow->show();
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

Entity<Components> * e;
void ExtrapolateAndConquer::initialize(void){

    loadResources();
    Renderer* r = graphicsWindow->getRenderer();

    Object* o1 = new Object(rm.getModel("teapot"),rm.getShader("phong"));

    //the skybox needs to be specially added to the renderer
    Object* skybox = new Object(rm.getModel("skybox"),rm.getShader("skyboxShader"),rm.getTexture("skybox0"));
    r->skybox = skybox;
    
    r->renderList.push_back(o1);

    /*
    // Initialize systems
    simplePhysicsSystem.initialize(entityManager);
    graphicsUpdateSystem.initialize(entityManager);
	
	// Initialize entity
    e = &entityManager.createEntity();
    e->add<SimplePhysics>();
    e->get<SimplePhysics>().position = QVector3D(0,0,0);
    e->get<SimplePhysics>().velocity = QVector3D(0,-0.01,0);
    e->add<Graphics>();
    e->get<Graphics>().object = new Object(rm.getModel("teapot"), rm.getShader("phong"));
    */
    


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
    WorldGen wg = WorldGen();
    Object* worldObject;

    int nOctaves = sizeof(octaves)/sizeof(float);
    world = wg.generateWorld(1000,1000,0.5f,octaves,scales,nOctaves);

    worldObject = new Object(world, rm.getShader("terrainShader"), rm.getTexture("grass"));
    worldObject->setShaderParameters(0.3, 0.7, 0.3, 50);
    worldObject->setColor(85,196,48,255);
    worldObject->setPosition(-500,0,-500);
    //worldObject->setTexScaling(1000);

    r->world = worldObject;
}

void ExtrapolateAndConquer::loadResources(void){
    
    //test data
    rm.loadShader("phong");
    rm.loadModel("teapot");

    //skybox data
    rm.loadModel("skybox");
    rm.loadTexture("skybox0");
    rm.loadShader("skyboxShader");

    rm.loadTexture("grass");
    rm.loadShader("terrainShader");
}

int ExtrapolateAndConquer::run(){
    
    initialize();
    timer->start(20);
    int returnCode = application->exec();
    return returnCode;
}

bool first = true;
void ExtrapolateAndConquer::loopBody(){
    cam->updatePosition();

    // Run the systems...
    //simplePhysicsSystem.batch();
    //graphicsUpdateSystem.batch();

    //make sure to update the gl widget...
    graphicsWindow->centralWidget()->update();
}

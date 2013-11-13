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

    r->renderList.push_back(e->get<Graphics>().object);
}

void ExtrapolateAndConquer::loadResources(void){
    
    //test data
    rm.loadShader("phong");
    rm.loadModel("teapot");

    //skybox data
    rm.loadModel("skybox");
    rm.loadTexture("skybox0");
    rm.loadShader("skyboxShader");
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
    simplePhysicsSystem.batch();
    graphicsUpdateSystem.batch();

    //make sure to update the gl widget...
    graphicsWindow->centralWidget()->update();
}

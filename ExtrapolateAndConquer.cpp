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

void ExtrapolateAndConquer::initialize(void){

    loadResources();
    Renderer* r = graphicsWindow->getRenderer();

    graphics::Object o1 = graphics::Object(rm.getModel("teapot"),rm.getShader("phong"));

    r->renderList.push_back(o1);
    graphics::Object* skybox = new graphics::Object(rm.getModel("skybox"),rm.getShader("skyboxShader"),rm.getTexture("skybox0"));
    r->skybox = skybox;
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

void ExtrapolateAndConquer::loopBody(){
    cam->updatePosition();

    //make sure to update the gl widget...
    graphicsWindow->centralWidget()->update();
}

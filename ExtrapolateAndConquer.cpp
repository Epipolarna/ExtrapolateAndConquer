#include "ExtrapolateAndConquer.hpp"

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[]){
    
    application = new QApplication(argc,argv);
    graphicsWindow = new GraphicsWindow();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));

    cam = graphicsWindow->getRenderer()->cam;

    graphicsWindow->registerEventManager(cam);
    timer->start(20);
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

void ExtrapolateAndConquer::loadResources(void){
    rm.loadShader("phong");
    rm.loadModel("teapot");
}

int ExtrapolateAndConquer::run(){
    
    graphicsWindow->show();
    loadResources();
    Renderer* r = graphicsWindow->getRenderer();

    graphics::Object o1 = graphics::Object(rm.getModel("teapot"),rm.getShader("phong"));

    r->renderList.push_back(o1);

    int returnCode = application->exec();
    return returnCode;
}

void ExtrapolateAndConquer::loopBody(){
    cam->updatePosition();

    //make sure to update the gl widget...
    graphicsWindow->centralWidget()->update();
}
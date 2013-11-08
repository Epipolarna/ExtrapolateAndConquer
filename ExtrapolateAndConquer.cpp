#include "ExtrapolateAndConquer.hpp"

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[]){
    
    application = new QApplication(argc,argv);
    graphicsWindow = new GraphicsWindow();
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));

    objectList = graphicsWindow->getRenderer()->objectList;

    cam = graphics::Camera();

    timer->start(20);
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

int ExtrapolateAndConquer::run(){

    graphicsWindow->show();
    int returnCode = application->exec();

    rm.loadShader("phong");
    rm.loadModel("teapot");

    objectList->push_back(graphics::Object(rm.getModel("teapot"),rm.getShader("phong")));

    return returnCode;
}

void ExtrapolateAndConquer::loopBody(){
    graphicsWindow->paintGL();
}

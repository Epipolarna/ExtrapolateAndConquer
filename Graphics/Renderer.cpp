#include "Renderer.hpp"

Renderer::Renderer(){

	objectList = new std::vector<graphics::Object>();
    cam = new graphics::Camera();
    cam->setPosition(QVector3D(0.0, 0.0, 0.0));
}

void Renderer::render(void){

    cam->updatePosition();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    //skybox->draw2(currentCamera->skyboxMatrix(), pMatrix);

    glEnable(GL_DEPTH_TEST);

    for(graphics::Object o : *objectList){
    	o.draw2(cam->vMatrix,pMatrix);
    }

    glEnable(GL_CULL_FACE);
}
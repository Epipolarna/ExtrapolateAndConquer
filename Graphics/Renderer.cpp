#include "Renderer.hpp"

Renderer::Renderer()
{
	objectList = new std::vector<graphics::Object>();
}

void Renderer::render(void){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    //skybox->draw2(currentCamera->skyboxMatrix(), pMatrix);

    glEnable(GL_DEPTH_TEST);

    for(graphics::Object o : *objectList){
    	o.draw2();
    }

    glEnable(GL_CULL_FACE);

}
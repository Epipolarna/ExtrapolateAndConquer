#include "Renderer.hpp"

Renderer::Renderer(){
    cam = new graphics::Camera();
}

void Renderer::repaint(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(skybox != NULL){
            skybox->draw(cam->skyboxMatrix(),pMatrix);
    }

    glEnable(GL_DEPTH_TEST);


    for(graphics::Object o : renderList){
        o.draw(cam->vMatrix,pMatrix);
    }

    glEnable(GL_CULL_FACE);
}

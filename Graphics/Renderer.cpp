#include "Renderer.hpp"

Renderer::Renderer(void){
    camera = new Camera();

}

void Renderer::drawObject(Object* o){
    this->renderList.push_back(o);
}

void Renderer::repaint(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    if(skybox != NULL){
        skybox->draw(camera->skyboxMatrix(),pMatrix);
    }

    glEnable(GL_DEPTH_TEST);

    if(world != NULL){
        world->draw(camera->vMatrix,pMatrix);
    }

    if(water != NULL){
        water->draw(camera->vMatrix,pMatrix);
    }

    for(Object * o : renderList){
<<<<<<< HEAD
        o->draw(cam->vMatrix,pMatrix);
=======
        //printf("Pointer to object is: %x \n",o);
        o->draw(camera->vMatrix,pMatrix);
>>>>>>> origin/rearchitecture
    }

    glEnable(GL_CULL_FACE);
}

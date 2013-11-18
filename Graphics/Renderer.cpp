#include "Renderer.hpp"

Renderer::Renderer(void){
    camera = new Camera();
}

void Renderer::drawObject(Object* o){
    //TODO batch data for instanced rendering somehow
    this->renderList.push_back(o);
}

void Renderer::drawObjects(Model* model,QOpenGLShaderProgram* program,std::vector<QVector3D> positions){
    
    program->bind();
    model->VAO.bind();
    model->VBO.bind();
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex",GL_FLOAT,0,3);
    model->NBO.bind();
    program->enableAttributeArray("normal");
    program->setAttributeBuffer("normal",GL_FLOAT,0,3);
    model->TBO.bind();
    program->enableAttributeArray("texCoord");
    program->setAttributeBuffer("texCoord",GL_FLOAT,0,2);
    
    std::vector<QMatrix4x4> mMatrices;
    //make position matrices...
    for(QVector3D pos : positions){
        QMatrix4x4 m = QMatrix4x4();
        m.setToIdentity();
        m.translate(pos);
        mMatrices.push_back(m);
    }

    program->setUniformValueArray("mMatrix",&mMatrices[0],mMatrices.size());
    program->setUniformValue("pMatrix",pMatrix);
    program->setUniformValue("vMatrix",camera->vMatrix);
    glDrawElementsInstanced(GL_TRIANGLES,model->index.size(),GL_UNSIGNED_INT,0L,mMatrices.size());

    model->VAO.release();
    program->release();
}

void Renderer::repaint(){

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(skybox != NULL){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        skybox->draw(camera->skyboxMatrix(),pMatrix);
    }

    if(world != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        world->draw(camera->vMatrix,pMatrix);
    }

    for(Object * o : renderList){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        o->draw(camera->vMatrix,pMatrix);
    }

    if(treeModel != NULL && treePositions.size() > 0){
        drawObjects(treeModel,treeShader,treePositions);
    }

    if(water != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        water->draw(camera->vMatrix,pMatrix);
        glDisable(GL_BLEND);
    }
}

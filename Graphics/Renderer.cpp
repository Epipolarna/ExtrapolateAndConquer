#include "Renderer.hpp"

Renderer::Renderer(void){
    camera = new Camera();
    initializeOpenGLFunctions();
}

void Renderer::drawObject(Object* o){
    //TODO batch data for instanced rendering somehow
    this->renderList.push_back(o);
}

void Renderer::drawObjects(Model* model,QOpenGLShaderProgram* program,std::vector<QVector3D> positions,GLuint tex){
    
    float ambientCoeff  = 0.2;
    float diffuseCoeff  = 0.6;
    float specularCoeff = 100;
    float specularExponent = 50;
    float texScaling = 1.0;

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

    program->setUniformValue("tex0", 0);
    program->setUniformValue("texScaling", texScaling);
    program->setUniformValue("ambientCoeff", ambientCoeff);
    program->setUniformValue("diffuseCoeff", diffuseCoeff);
    program->setUniformValue("specularCoeff", specularCoeff);
    program->setUniformValue("specularExponent", specularExponent);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex);

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

void Renderer::initFBO()
{
    /*
    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
            qWarning("This system has no framebuffer object support");
    }
    // QGLFramebufferObject::Depth enables the depth buffer for the FBO
    fboFormat.setInternalTextureFormat(GL_RGBA16F);
    fboFormat.setAttachment(QGLFramebufferObject::Depth);
    fboFormat.setSamples(0);

    FBO1 = new QGLFramebufferObject(width(), height(), fboFormat);
    FBO2 = new QGLFramebufferObject(width(), height(), fboFormat);
    FBO3 = new QGLFramebufferObject(width(), height(), fboFormat);
    */
}

void Renderer::useFBO(QGLFramebufferObject *FBO)
{
    /*
    FBO->bind();
    // Since the GL_TEXTURE? cannot be found in QGLFBO it has to
    // expressed relative GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0 + FBO->texture());
    glBindTexture(GL_TEXTURE_2D, FBO->texture());
    */
}

void Renderer::repaint(){

    // -------------- FBO ----------------------------
    /*
    // Draw scene to the FBO1
    useFBO(FBO1);
    //glViewport(???); might be needed to support resize
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    player->setPosition(1,0,-3);
    object->draw(player, pMatrix);
    //QImage im1 = FBO1->toImage();
    //im1.save("im1.png");

    useFBO(FBO2);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square->program = threshShader;
    square->draw(player, pMatrix, FBO1->texture());
    //QImage im2 = FBO2->toImage();
    //im2.save("im2.png");

    // Draw the FBO to a square and display
    QGLFramebufferObject::bindDefault();    // Set the "normal" screen as render target
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    square->program = texShader;
    square->draw(player, pMatrix, FBO2->texture());
    */
    // -----------------------------------------------

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

    if(water != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        water->draw(camera->vMatrix,pMatrix);
        glDisable(GL_BLEND);
    }

    /*
    if(treeModel != NULL && treePositions.size() > 0){
        glEnable(GL_BLEND);
        drawObjects(treeModel,treeShader,treePositions,treeTexture);
        glDisable(GL_BLEND);
    }
    */
}

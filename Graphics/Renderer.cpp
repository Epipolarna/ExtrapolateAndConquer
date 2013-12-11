#include "Renderer.hpp"


Renderer::Renderer(void){
    camera = new Camera();
    width = 1280;
    height = 800;
    shadowMapSize = 2048;
    initializeOpenGLFunctions();
    fbo1 = new FBO;
    fbo2 = new FBO;
    fbo3 = new FBO;
    initFBO(fbo1);
    initFBO(fbo2);
    initFBO(fbo3);
}

void Renderer::drawObject(Object* o){
    //TODO batch data for instanced rendering somehow
    this->renderList.push_back(o);
}

void Renderer::drawInstanceObjects(StaticObjectList* statics){

    QOpenGLShaderProgram* program = statics->getProgram();
    Model* model = statics->getModel();
    GLuint tex = statics->getTextures()[0];

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


    program->setUniformValue("pMatrix",pMatrix);
    program->setUniformValue("vMatrix",camera->vMatrix);
    QVector<QMatrix4x4> translations = statics->getMatrices();

    for(int i=0; i < translations.size(); i = i + maxInstanceObjects){
        int offset = maxInstanceObjects;
        if(i + offset > translations.size()){
            offset = translations.size() - i;
        }
        program->setUniformValueArray("mMatrix",&translations[i],offset);
        glDrawElementsInstanced(GL_TRIANGLES,model->index.size(),GL_UNSIGNED_INT,0L,offset);
    }

    model->VAO.release();
    program->release();
}

void Renderer::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Renderer::initFBO(FBO* fbo)
{
    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
            qWarning("This system has no framebuffer object support");
    }

    // ------- Classic OpenGL ---------------------------

    glGenFramebuffers(1, &fbo->id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);

    // ColorBuffer
    glGenTextures(1, &fbo->colorTex);
    glBindTexture(GL_TEXTURE_2D, fbo->colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowMapSize, shadowMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->colorTex, 0);

    // DepthBuffer
    glGenTextures(1, &fbo->depthTex);
    glBindTexture(GL_TEXTURE_2D, fbo->depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0L);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depthTex, 0);

    // Set current buffer to default
    QGLFramebufferObject::bindDefault();
}

void Renderer::useFBO(FBO* fbo)
{
    if(fbo == 0){
    // Bind default
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0, width, height);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo->id);
        // Since the GL_TEXTURE? cannot be found in QGLFBO it has to
        // expressed relative GL_TEXTURE0
        glActiveTexture(GL_TEXTURE0 + fbo->colorTex);
        glBindTexture(GL_TEXTURE_2D, fbo->colorTex);
        glActiveTexture(GL_TEXTURE0 + fbo->depthTex);
        glBindTexture(GL_TEXTURE_2D, fbo->depthTex);

        glViewport(0,0, shadowMapSize, shadowMapSize);
    }
}

void Renderer::calculateLightSourceMatrices()
{
    /*
    qDebug() << "CamPos:" << camera->position;
    qDebug() << "CamLightSpacePos:" << lightSourceVMatrix*camera->position;
    */
    QVector4D lightSpaceVertex;
    float minX = 1000000;
    float maxX = 0.2;
    float minY = 1000000;
    float maxY = 0.2;
    float minZ = 1000000;
    float maxZ = 0.2;
    float lsX = 0;
    float lsY = 0;
    float lsZ = 0;
    bool okInv = false;

    for(int i = 1; i < frustumCorners.size(); i++){
        QVector4D worldSpaceVertex = frustumCorners[i];

        lightSpaceVertex = lightSourceVMatrix*camera->vMatrix.inverted(&okInv)*worldSpaceVertex;
        lsX = lightSpaceVertex.x();
        lsY = lightSpaceVertex.y();
        lsZ = -lightSpaceVertex.z();

        // X
        if(lsX > maxX){
            maxX = lsX;
        }
        if(lsX < minX){
            minX = lsX;
        }

        // Y
        if(lsY > maxY){
            maxY = lsY;
        }
        if(lsY < minY){
            minY = lsY;
        }

        // Z
        if(lsZ > 0){
            if(lsZ > maxZ){
                maxZ = lsZ;
            }
            if(lsZ < minZ){
                minZ = lsZ;
            }
        } else {
            minZ = 0.1;
        }
    }
/*
    qDebug() << "left" << minX;
    qDebug() << "right" << maxX;
    qDebug() << "bottom" << minY;
    qDebug() << "top" << maxY;
    qDebug() << "NearPlane" << minZ;
    qDebug() << "FarPlane" << maxZ;
*/
    lightSourcePMatrix.setToIdentity();
    lightSourcePMatrix.ortho(-120,120,-20,150,50,maxZ);
    //lightSourcePMatrix.ortho(minX,maxX,minY,maxY,minZ,maxZ);
}

void Renderer::repaint(){

    calculateLightSourceMatrices();

    // Draw the scene from the lightsource to shadowMap FBO
    useFBO(fbo1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if(world != NULL){
        world->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
    }

    for(Object * o : renderList){
        o->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
    }

    // TODO: Draw tree batch stuff to shadowMap to enable tree shadows

    if(water != NULL){
        water->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
    }

    glCullFace(GL_BACK);

    // Draw everything again to the default FBO
    useFBO(0); // Default

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(skybox != NULL){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        skybox->draw(camera->skyboxMatrix(),pMatrix,lightPosition,lightSourceVMatrix);
    }

    if(world != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        world->program->bind();
        world->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
        world->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
        world->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
    }

    for(Object * o : renderList){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        o->program->bind();
        o->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
        o->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
        o->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
    }

    // Used to move the waves
    static float incr = 0;
    incr += 0.0005;
    incr = incr > 1 ? incr-1 : incr;


    if(water != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        water->program->bind();
        water->program->setUniformValue("incr", incr);
        water->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
        water->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
        water->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
        glDisable(GL_BLEND);
    }

    if(worldData != NULL){
        glEnable(GL_BLEND);
        drawInstanceObjects(worldData->trees);
        glDisable(GL_BLEND);
    }
    

    //QImage im1 = FBO1->toImage();
    //im1.save("im1.png");

    // Render the FBO to the default buffer
    /*
    QGLFramebufferObject::bindDefault();    // Set the "normal" screen as render target
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    fboSquare->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
    */


}

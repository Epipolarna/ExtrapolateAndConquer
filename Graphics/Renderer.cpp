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

    //make the matrix array
    std::vector<QMatrix4x4> translations = std::vector<QMatrix4x4>(positions.size());
    for(QVector3D p : positions){
        QMatrix4x4 m = QMatrix4x4();
        m.setToIdentity();
        m.translate(p);
        translations.push_back(m);
    }

    //draw in chunks of maxInstanceObjects
    for(int i=0; i < translations.size(); i = i + maxInstanceObjects){

        int lastIndex = i + maxInstanceObjects;
        if(lastIndex > translations.size()){
            lastIndex = translations.size() - i;
        }

        int offset = lastIndex - i;

        program->setUniformValueArray("mMatrix",&translations[i],offset);
        program->setUniformValue("pMatrix",pMatrix);
        program->setUniformValue("vMatrix",camera->vMatrix);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, shadowMapSize, shadowMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->colorTex, 0);

    // DepthBuffer
    glGenTextures(1, &fbo->depthTex);
    glBindTexture(GL_TEXTURE_2D, fbo->depthTex);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0L);
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

void Renderer::repaint(){

    // Draw the scene from the lightsource to shadowMap FBO
    useFBO(fbo1);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    if(world != NULL){
        world->customDraw(lightSourceVMatrix,pMatrix,depthProgram);
    }

    for(Object * o : renderList){
        o->customDraw(lightSourceVMatrix,pMatrix,depthProgram);
    }

    if(water != NULL){
        water->customDraw(lightSourceVMatrix,pMatrix,depthProgram);
    }

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
        world->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
    }

    for(Object * o : renderList){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
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
        water->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
        glDisable(GL_BLEND);
    }

    
    if(treeModel != NULL && treePositions.size() > 0){
        glEnable(GL_BLEND);
        drawObjects(treeModel,treeShader,treePositions,treeTexture);
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

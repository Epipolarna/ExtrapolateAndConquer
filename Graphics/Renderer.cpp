#include "Renderer.hpp"

Renderer::Renderer(void){
    camera = new Camera();
    width = 1280;
    height = 800;
    initializeOpenGLFunctions();
    initFBO();
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

void Renderer::setSize(int width, int height)
{
    this->width = width;
    this->height = height;
}

void Renderer::initFBO()
{
    if (!QGLFramebufferObject::hasOpenGLFramebufferObjects()) {
            qWarning("This system has no framebuffer object support");
    }

    // ------- Classic OpenGL ---------------------------

    glGenFramebuffers(1, &fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    // ColorBuffer
    glGenTextures(1, &fboColorTex);
    glBindTexture(GL_TEXTURE_2D, fboColorTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboColorTex, 0);

    // DepthBuffer
    glGenTextures(1, &fboDepthTex);
    glBindTexture(GL_TEXTURE_2D, fboDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0L);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fboDepthTex, 0);

    //glGenRenderbuffers(1, &fboDepth);
    //glBindRenderbuffer(GL_RENDERBUFFER, fboDepth);

    // Set current buffer to default
    QGLFramebufferObject::bindDefault();
}

void Renderer::useFBO()
{
    //FBO->bind();
    // Since the GL_TEXTURE? cannot be found in QGLFBO it has to
    // expressed relative GL_TEXTURE0
    //glActiveTexture(GL_TEXTURE0 + FBO->texture());
    //glBindTexture(GL_TEXTURE_2D, FBO->texture());

    glBindFramebuffer(GL_FRAMEBUFFER, fboID);
}

void Renderer::repaint(){

    // Draw scene to the FBO1
    useFBO();

    qDebug() << lightPosition;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(skybox != NULL){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        skybox->draw(camera->skyboxMatrix(),pMatrix,lightPosition);
    }

    if(world != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        world->draw(camera->vMatrix,pMatrix,lightPosition);
    }

    for(Object * o : renderList){
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        o->draw(camera->vMatrix,pMatrix,lightPosition);
    }

    // Debug & testing
    static float wave = 0;
    static float incr = 0;

    wave += 0.01;
    wave = wave > 2*3.1415 ? wave-2*3.1415 : wave;

    incr += 0.0005;
    incr = incr > 1 ? incr-1 : incr;


    if(water != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        water->program->bind();
        water->program->setUniformValue("wave", wave);
        water->program->setUniformValue("incr", incr);
        water->draw(camera->vMatrix,pMatrix,lightPosition);
        glDisable(GL_BLEND);
    }

    /*
    if(treeModel != NULL && treePositions.size() > 0){
        glEnable(GL_BLEND);
        drawObjects(treeModel,treeShader,treePositions,treeTexture);
        glDisable(GL_BLEND);
    }
    */

    //QImage im1 = FBO1->toImage();
    //im1.save("im1.png");

    // Render the FBO to the default buffer

    QGLFramebufferObject::bindDefault();    // Set the "normal" screen as render target
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    fboSquare->draw(camera->vMatrix,pMatrix,lightPosition);
}

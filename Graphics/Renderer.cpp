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

    shadowLevel = 1;

    isRenderingTerrain = false;
    isRenderingTrees = false;
    isRenderingBalls = false;
    isRenderingShadows = false;

    isPCF = true;

    isRenderingTerrain = isRenderingTrees = isRenderingBalls = isRenderingShadows = true;
}

void Renderer::drawObject(Object* o){
    //TODO batch data for instanced rendering somehow
    this->renderList.push_back(o);
}

void Renderer::drawInstanceObjects(StaticObjectList* statics, int renderToDepthMap){

    QOpenGLShaderProgram* program;
    if(renderToDepthMap > 0){
        program = instanceDepthProgram;
    } else {
        program = statics->getProgram();
    }

    Model* model = statics->getModel();
    GLuint tex = statics->getTextures()[0];

    float ambientCoeff  = 0.2;
    float diffuseCoeff  = 0.6;
    float specularCoeff = 0.2;
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

    if(renderToDepthMap == 1){
        program->setUniformValue("pMatrix",lightSourcePMatrix1);
        program->setUniformValue("vMatrix",lightSourceVMatrix);
    } else if(renderToDepthMap == 2){
        program->setUniformValue("pMatrix",lightSourcePMatrix2);
        program->setUniformValue("vMatrix",lightSourceVMatrix);
    } else if(renderToDepthMap == 3){
        program->setUniformValue("pMatrix",lightSourcePMatrix3);
        program->setUniformValue("vMatrix",lightSourceVMatrix);
    } else {
        program->setUniformValue("pMatrix",pMatrix);
        program->setUniformValue("vMatrix",camera->vMatrix);
    }
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
    GLfloat borderColor[4]={1.0,1.0,1.0,1.0};
    //GLfloat borderColor[4]={0.0,0.0,0.0,0.0};
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depthTex, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    lightSourceVMatrix.setToIdentity();
    lightSourcePMatrix.setToIdentity();
    lightSourcePMatrix1.setToIdentity();
    lightSourcePMatrix2.setToIdentity();
    lightSourcePMatrix3.setToIdentity();

    QVector3D pos = camera->position;
    pos.setY(std::max(worldData->getHeight(pos.x(),pos.z()), 0.0f));
    //pos.setY(0);

    lightSourceVMatrix.lookAt(lightPosition, pos, QVector3D(0,1,0));

    int frstumWidth;
    int frstumHeight;


    frstumWidth = 150;
    frstumHeight = 150;
    lightSourcePMatrix1.ortho(-frstumWidth,frstumWidth,-frstumHeight,frstumHeight,50,1000);

    frstumWidth = 50;
    frstumHeight = 50;
    lightSourcePMatrix2.ortho(-frstumWidth,frstumWidth,-frstumHeight,frstumHeight,50,1000);

    frstumWidth = 10;
    frstumHeight = 10;
    lightSourcePMatrix3.ortho(-frstumWidth,frstumWidth,-frstumHeight,frstumHeight,50,1000);

    lightSourcePMatrix = lightSourcePMatrix1;
}

void Renderer::repaint(){

    // Used to move the waves (etc)
    static float incr = 0.0;
    //incr += 0.0005;
    float speed = 0.07;
    incr += dt*speed;
    incr = incr > 1 ? incr-1 : incr;

    // ----------- SHADOW MAPPING -------------------------------
    // Draw the scene from the lightsource to shadowMap FBO
    calculateLightSourceMatrices();
    for(int i = 1; i <= 3; i++){

        switch(i){
        case 1:
            useFBO(fbo1);
            lightSourcePMatrix = lightSourcePMatrix1;
            break;
        case 2:
            useFBO(fbo2);
            lightSourcePMatrix = lightSourcePMatrix2;
            break;
        case 3:
            useFBO(fbo3);
            lightSourcePMatrix = lightSourcePMatrix3;
            break;
        default:
            useFBO(fbo1);
            lightSourcePMatrix = lightSourcePMatrix1;
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(isRenderingShadows){


        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        // TODO: Draw tree batch stuff to shadowMap to enable tree shadows

            if(isRenderingTrees){
                if(worldData != NULL){
                    drawInstanceObjects(worldData->tree1, i);
                    drawInstanceObjects(worldData->leaf1, i);

                    drawInstanceObjects(worldData->tree2, i);
                    drawInstanceObjects(worldData->leaf2, i);

                    drawInstanceObjects(worldData->tree3, i);
                    drawInstanceObjects(worldData->leaf3, i);

                    drawInstanceObjects(worldData->bush1, i);
                    drawInstanceObjects(worldData->bush2, i);
                }
            }


            if(isRenderingTerrain){
                if(world != NULL){
                    world->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
                }
            }

            if(isRenderingBalls){
                for(Object * o : renderList){
                    o->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
                }
            }

            if(water != NULL){
                water->customDraw(lightSourceVMatrix,lightSourcePMatrix,depthProgram);
            }

            glCullFace(GL_BACK);
        }
    }

    lightSourcePMatrix = lightSourcePMatrix1;

    // ----------- STANDARD RENDER -------------------------------
    // Draw everything again to the default FBO
    useFBO(0); // Default

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(skybox != NULL){
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        skybox->draw(camera->skyboxMatrix(),pMatrix,lightPosition,lightSourceVMatrix);
    }

    if(isRenderingTerrain){
        if(world != NULL){

            glEnable(GL_DEPTH_TEST);
            glDisable(GL_CULL_FACE);

            world->program->bind();
            world->program->setUniformValue("incr", incr);
            world->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
            world->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
            world->program->setUniformValue("lightSourcePMatrix1", lightSourcePMatrix1);
            world->program->setUniformValue("lightSourcePMatrix2", lightSourcePMatrix2);
            world->program->setUniformValue("lightSourcePMatrix3", lightSourcePMatrix3);
            world->program->setUniformValue("tex4", 4);        // Shadow map
            world->program->setUniformValue("tex5", 5);        // Shadow map
            world->program->setUniformValue("cameraPosition", camera->position);

            world->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
        }
    }

    if(isRenderingBalls){
        for(Object * o : renderList){
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            o->program->bind();
            o->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
            o->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
            o->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
        }
    }


    if(water != NULL){
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        water->program->bind();
        water->program->setUniformValue("incr", incr);
        water->program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
        water->program->setUniformValue("lightSourcePMatrix", lightSourcePMatrix);
        water->program->setUniformValue("lightSourcePMatrix1", lightSourcePMatrix1);
        water->program->setUniformValue("lightSourcePMatrix2", lightSourcePMatrix2);
        water->program->setUniformValue("lightSourcePMatrix3", lightSourcePMatrix3);
        water->program->setUniformValue("tex4", 4);        // Shadow map
        water->program->setUniformValue("tex5", 5);        // Shadow map
        water->program->setUniformValue("cameraPosition", camera->position);
        water->draw(camera->vMatrix,pMatrix,lightPosition,lightSourceVMatrix);
        glDisable(GL_BLEND);
    }

    if(isRenderingTrees){
        if(worldData != NULL){
            glEnable(GL_BLEND);
            drawInstanceObjects(worldData->tree1, false);
            drawInstanceObjects(worldData->leaf1, false);

            drawInstanceObjects(worldData->tree2, false);
            drawInstanceObjects(worldData->leaf2, false);

            drawInstanceObjects(worldData->tree3, false);
            drawInstanceObjects(worldData->leaf3, false);


            //do not z fight!
            glDepthMask (GL_FALSE);
            drawInstanceObjects(worldData->bush2, false);
            drawInstanceObjects(worldData->bush1, false);
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);
        }
    }
}

void Renderer::sortOnDepth(std::vector<Object *> & objects, int iterations)
{
    bool swapped = true;

    // "iterations" iterations of bubblesort according to the xyz distance to the camera goes here <-!
    for(int i = 0; i < iterations && swapped; i++) {
        for(int n = 0; n < objects.size()-1; n++) {
            if(closerThan(objects[n], objects[n+1])) {
                std::swap(objects[n], objects[n+1]);
                swapped = true;
            }
        }
    }
}

bool Renderer::closerThan(Object * obj1, Object *obj2) {
    float distance1 = (camera->position - obj1->getPosition()).length();
    float distance2 = (camera->position - obj2->getPosition()).length();
    return distance1 < distance2;
}

#include "Object.hpp"


Object::Object(Model *_model, QOpenGLShaderProgram *_program, GLuint _texture){
    model = _model;
    textures.push_back(_texture);
    program = _program;

    mMatrix.setToIdentity();
    position = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;

    color = QVector4D(1,1,1,1);
}

Object::Object(Model *_model, QOpenGLShaderProgram *_program, QVector<GLuint> textures){
    model = _model;
    this->textures = QVector<GLuint>(textures);
    program = _program;

    mMatrix.setToIdentity();
    position = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;

    color = QVector4D(1,1,1,1);
}

void Object::setShaderParameters(float ambientCoeff,float diffuseCoeff,float specularCoeff,float specularExponent){
    this->ambientCoeff  = ambientCoeff;
    this->diffuseCoeff  = diffuseCoeff;
    this->specularCoeff = specularCoeff;
    this->specularExponent = specularExponent;  
}

void Object::draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix){

    program->bind();
    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("tex", 0);
    program->setUniformValue("scale", scale);
    program->setUniformValue("color", color);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    if(model->VAO.isCreated()){
        model->VAO.bind();
    }else{
        printf("VAO is not created!! \n");
        exit(0);
    }

    if(model->VBO.isCreated()){
        model->VBO.bind();
        program->enableAttributeArray("vertex");
        program->setAttributeBuffer("vertex",GL_FLOAT,0,3);
    }else{
        printf("VBO is not created!! \n");
        exit(0);
    }

    if(model->NBO.isCreated()){
        model->NBO.bind();
        program->enableAttributeArray("normal");
        program->setAttributeBuffer("normal",GL_FLOAT,0,3);
    }else{
        printf("NBO is not created!! \n");
        exit(0);
    }

    if(model->TBO.isCreated()){
        model->TBO.bind();
        program->enableAttributeArray("texCoord");
        program->setAttributeBuffer("texCoord",GL_FLOAT,0,2);
    }else{
        printf("TBO is not created!! \n");
        exit(0);
    }
    
    glDrawElements(GL_TRIANGLES,model->index.size(),GL_UNSIGNED_INT,0L);

    model->VAO.release();
    program->release();
}


void Object::setPosition(float x, float y, float z)
{
    position = QVector3D(x,y,z);
    mMatrix.setToIdentity();
    mMatrix.translate(position);
    mMatrix.scale(scale);
}

void Object::setScale(float x, float y, float z)
{
    scale = QVector3D(x,y,z);
    mMatrix.setToIdentity();
    mMatrix.translate(position);
    mMatrix.scale(scale);
}

void Object::setColor(float r, float g, float b, float a)
{
    color = QVector4D(r/255,g/255,b/255,a/255);
}



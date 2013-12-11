#include "Object.hpp"

const int Object::textureSlots[] = {GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2, GL_TEXTURE3};


Object::Object(Model *_model, QOpenGLShaderProgram *_program, GLuint _texture){
    model = _model;
    textures.push_back(_texture);
    program = _program;

    initVariables();
}

Object::Object(Model *_model, QOpenGLShaderProgram *_program, QVector<GLuint> textures){
    model = _model;
    this->textures = QVector<GLuint>(textures);
    this->program = _program;

    initVariables();
}

void Object::initVariables(void){
    mMatrix.setToIdentity();
    position = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);
    rotation = QQuaternion(1,0,0,0);

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;

    texScaling = 1.0;

    color = QVector4D(1,1,1,1);
    initializeOpenGLFunctions();
}

void Object::setShaderParameters(float ambientCoeff,float diffuseCoeff,float specularCoeff,float specularExponent){
    this->ambientCoeff  = ambientCoeff;
    this->diffuseCoeff  = diffuseCoeff;
    this->specularCoeff = specularCoeff;
    this->specularExponent = specularExponent;
}

void Object::setTexScaling(float s){
    this->texScaling = s;
}


QVector3D Object::getPosition(){
    return position;
}
Model* Object::getModel(){
    return model;
}

void Object::draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix, QVector3D lightPosition, QMatrix4x4 lightSourceVMatrix){

    program->bind();
    program->setUniformValue("lightPosition", lightPosition);
    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("lightSourceVMatrix", lightSourceVMatrix);
    program->setUniformValue("tex0", 0);        // Main tex
    program->setUniformValue("tex1", 1);        // Blend tex
    program->setUniformValue("tex2", 2);        // Normal map
    program->setUniformValue("tex3", 3);        // Shadow map
    program->setUniformValue("texScaling", texScaling);
    program->setUniformValue("scale", scale);
    program->setUniformValue("color", color);
    program->setUniformValue("ambientCoeff", ambientCoeff);
    program->setUniformValue("diffuseCoeff", diffuseCoeff);
    program->setUniformValue("specularCoeff", specularCoeff);
    program->setUniformValue("specularExponent", specularExponent);
    program->setUniformValue("specialValue", specialValue);

    for(int i=0; i < textures.size(); ++i){
        glActiveTexture(textureSlots[i]);
        glBindTexture(GL_TEXTURE_2D,textures[i]);
    }



    glActiveTexture(GL_TEXTURE0);


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

extern float specialValue = 1.0;

void Object::customDraw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix, QOpenGLShaderProgram *program)
{
    program->bind();
    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("scale", scale);
    program->setUniformValue("specialValue", specialValue);

    glActiveTexture(GL_TEXTURE0);


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

    glDrawElements(GL_TRIANGLES,model->index.size(),GL_UNSIGNED_INT,0L);

    model->VAO.release();
    program->release();
}

void Object::setPosition(float x, float y, float z)
{
    position = QVector3D(x,y,z);
    uppdateTransform();
}

void Object::setPosition(QVector3D & position) {
    this->position = position;
    uppdateTransform();
}

void Object::setScale(float s) {
    scale = QVector3D(s,s,s);
    uppdateTransform();
}

void Object::setScale(float x, float y, float z)
{
    scale = QVector3D(x,y,z);
    uppdateTransform();
}

void Object::setScale(QVector3D & scale) {
    this->scale = scale;
    uppdateTransform();
}

void Object::setRotation(QVector3D & axis, float angle) {
    this->rotation = QQuaternion::fromAxisAndAngle(axis, angle);
    uppdateTransform();
}

void Object::setRotation(QQuaternion & rotation) {
    this->rotation = rotation;
}

void Object::uppdateTransform() {
    mMatrix.setToIdentity();
    mMatrix.translate(position);
    mMatrix.rotate(rotation);
    mMatrix.scale(scale);

}

void Object::setColor(float r, float g, float b, float a)
{
    color = QVector4D(r/255,g/255,b/255,a/255);
}



#include "Object.hpp"

namespace graphics {

Object::Object(Model *_model, QOpenGLShaderProgram *_program, GLuint _texture0, GLuint _texture1)
{
    initializeOpenGLFunctions();

    model = _model;
    texture0 = _texture0;
    texture1 = _texture1;
    program = _program;

    mMatrix.setToIdentity();
    position = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);

    ambientCoeff  = 0.4;
    diffuseCoeff  = 0.4;
    specularCoeff = 0.5;
    specularExponent = 50;

    color = QVector4D(1,1,1,1);
}

Object::Object(ModelLoader *_model, QOpenGLShaderProgram *_program, GLuint _texture0, GLuint _texture1)
{
    printf("using alternate object constructor \n");
    model2 = _model;
    texture0 = _texture0;
    texture1 = _texture1;
    program = _program;

    mMatrix.setToIdentity();
    position = QVector3D(0,0,0);
    scale = QVector3D(1,1,1);

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 0.2;
    specularExponent = 50;

    color = QVector4D(1,1,1,1);
}

void Object::draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix)
{
    program->bind();

    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("tex0", 0);
    program->setUniformValue("tex1", 1);
    program->setUniformValue("scale", scale);
    program->setUniformValue("color", color);
    program->setUniformValue("ambientCoeff", ambientCoeff);
    program->setUniformValue("diffuseCoeff", diffuseCoeff);
    program->setUniformValue("specularCoeff", specularCoeff);
    program->setUniformValue("specularExponent", specularExponent);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glActiveTexture(GL_TEXTURE0);

    if(model->VAO.isCreated()){
        model->VAO.bind();

        if(model->VBO.isCreated()){
            model->VBO.bind();
            program->enableAttributeArray("vertex");
            program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);

            if(model->NBO.isCreated()){
                model->NBO.bind();
                program->enableAttributeArray("normal");
                program->setAttributeBuffer("normal", GL_FLOAT, 0, 3);
            }

            if(model->TBO.isCreated()){
                model->TBO.bind();
                program->enableAttributeArray("texCoord");
                program->setAttributeBuffer("texCoord", GL_FLOAT, 0, 3);
            }

            if(model->groups[0].indices.size() > 0){
                glDrawElements(GL_TRIANGLES, model->groups[0].indices.size(), GL_UNSIGNED_INT, 0);
            } else {
                glDrawArrays(GL_TRIANGLES, 0, model->groups[0].vertices.size());
            }
        }
    }
    model->VAO.release();
    program->release();
}

//TODO move to main draw method
void Object::draw2(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix){

    program->bind();
    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("tex0", 0);
    program->setUniformValue("tex1", 1);
    program->setUniformValue("scale", scale);
    program->setUniformValue("color", color);

    glBindTexture(GL_TEXTURE_2D, texture0);
    if(model2->VAO.isCreated()){
        model2->VAO.bind();
    }else{
        printf("VAO is not created!! \n");
        exit(0);
    }

    if(model2->VBO.isCreated()){
        model2->VBO.bind();
        program->enableAttributeArray("vertex");
        program->setAttributeBuffer("vertex",GL_FLOAT,0,3);
    }else{
        printf("VBO is not created!! \n");
        exit(0);
    }

    if(model2->NBO.isCreated()){
        model2->NBO.bind();
        program->enableAttributeArray("normal");
        program->setAttributeBuffer("normal",GL_FLOAT,0,3);
    }else{
        printf("NBO is not created!! \n");
        exit(0);
    }

    if(model2->TBO.isCreated()){
        model2->TBO.bind();
        program->enableAttributeArray("texCoord");
        program->setAttributeBuffer("texCoord",GL_FLOAT,0,2);
    }else{
        printf("TBO is not created!! \n");
        exit(0);
    }
    
    glDrawElements(GL_TRIANGLES,model2->index.size(),GL_UNSIGNED_INT,0L);

    model2->VAO.release();
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

}


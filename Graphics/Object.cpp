#include "Object.hpp"
#include <QOpenGLFunctions>

namespace graphics {

Object::Object(Model *_model, QGLShaderProgram *_program, GLuint _texture)
{
    model = _model;
    texture = _texture;
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

void Object::draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix)
{
    program->bind();

    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("tex", 0);
    program->setUniformValue("scale", scale);
    program->setUniformValue("color", color);

    glBindTexture(GL_TEXTURE_2D, texture);

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

            glDrawArrays(GL_TRIANGLES, 0, model->groups[0].vertices.size());
        }
    }
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


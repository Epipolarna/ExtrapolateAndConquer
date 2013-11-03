#include "Object.hpp"
#include <QOpenGLFunctions>

namespace graphics {

Object::Object(Model *_model, QGLShaderProgram *_program, GLuint _texture)
{
    model = _model;
    texture = _texture;
    program = _program;

    mMatrix.setToIdentity();
    scale = 1;

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;
}

void Object::draw(QMatrix4x4 &vMatrix, QMatrix4x4 &pMatrix)
{
    program->bind();

    program->setUniformValue("mvpMatrix", pMatrix*vMatrix*mMatrix);
    program->setUniformValue("mMatrix", mMatrix);
    program->setUniformValue("vMatrix", vMatrix);
    program->setUniformValue("pMatrix", pMatrix);
    program->setUniformValue("tex", 0);
    program->setUniformValue("scale", scale);

    glBindTexture(GL_TEXTURE_2D, texture);

    model->VAO.bind();

    model->VBO.bind();
    program->enableAttributeArray("vertex");
    program->setAttributeBuffer("vertex", GL_FLOAT, 0, 3);

    model->NBO.bind();
    program->enableAttributeArray("normal");
    program->setAttributeBuffer("normal", GL_FLOAT, 0, 3);

    model->TBO.bind();
    program->enableAttributeArray("texCoord");
    program->setAttributeBuffer("texCoord", GL_FLOAT, 0, 3);

    glDrawArrays(GL_TRIANGLES, 0, model->groups[0].vertices.size());

    program->release();
}

void Object::setScale(float _scale)
{
    scale = _scale;
    mMatrix.scale(scale);
}

}


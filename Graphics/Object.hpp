#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Model.hpp"
#include "Camera.hpp"
#include <QOpenGLFunctions>

namespace graphics {

class Object : protected QOpenGLFunctions
{
public:
    Object(Model* model, QGLShaderProgram* program, GLuint texture = 0);

    void draw(QMatrix4x4 &vMatrix, QMatrix4x4 &pMatrix);

    QGLShaderProgram* program;
private:
    Model* model;
    GLuint texture;

    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float specularExponent;

    QVector3D position;
    QMatrix4x4 mMatrix;
};

}

#endif // OBJECT_HPP

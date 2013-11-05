#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Model.hpp"
#include "Camera.hpp"
#include "ModelLoader.hpp"

#include <QOpenGLFunctions>

namespace graphics {

class Object : protected QOpenGLFunctions
{
public:
    Object(Model* model, QGLShaderProgram* program, GLuint texture = 0);
    Object(ModelLoader* model, QGLShaderProgram* program, GLuint texture = 0);

    void draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix);
    void draw2(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix);

    QGLShaderProgram* program;

    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setColor(float r, float g, float b, float a = 1);

private:
    Model* model;
    ModelLoader* model2;
    GLuint texture;

    QVector3D position;
    QVector3D scale;

    QMatrix4x4 mMatrix;

    QVector4D color;

    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float specularExponent;
};

}

#endif // OBJECT_HPP

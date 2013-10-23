#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Model.hpp"
#include "Camera.hpp"
#include <QOpenGLFunctions>

namespace graphics {

class Object : protected QOpenGLFunctions
{
public:
    Object(Model* model, QGLShaderProgram* program, QGLContext* context);

    void draw(QMatrix4x4 &vMatrix, QMatrix4x4 &pMatrix);

    QGLShaderProgram* program;
private:
    QGLContext* context;
    Model* model;
    GLuint texture;

    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float specularExponent;

    QVector3D position;
    QMatrix4x4 mMatrix;

    int vertexAttr;
    int normalAttr;
    int texCoordAttr;
    int textureUniform;
    int mvpMatUniform;
    int mMatUniform;
    int vMatUniform;
    int pMatUniform;
    int ambientCoeffUniform;
    int diffuseCoeffUniform;
    int specularCoeffUniform;
    int specularExponentUniform;
};

}

#endif // OBJECT_HPP

#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "Camera.hpp"
#include "Model.hpp"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

class Object : protected QOpenGLFunctions
{
public:
    Object(Model* model, QOpenGLShaderProgram* program, GLuint texture = 0);
    Object(Model* model, QOpenGLShaderProgram* program, QVector<GLuint> textures);

    void setShaderParameters(float ambientCoeff,float diffuseCoeff,float specularCoeff,float specularExponent);
    void draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix);

    QOpenGLShaderProgram* program;

    void setPosition(float x, float y, float z);
    void setScale(float x, float y, float z);
    void setColor(float r, float g, float b, float a = 1);
    void setTexScaling(float s);

private:

    void initVariables(void);

    Model* model;
    QVector<GLuint> textures;

    QVector3D position;
    QVector3D scale;

    QMatrix4x4 mMatrix;

    QVector4D color;

    //shader data
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float specularExponent;

    float texScaling;


    //acual data in .cpp file.
    static const int textureSlots[];
};


#endif // OBJECT_HPP

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
    void draw(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix, QVector3D lightPosition);
    void drawDepth(const QMatrix4x4 &vMatrix, const QMatrix4x4 &pMatrix, QVector3D lightPosition);

    QOpenGLShaderProgram* program, *depthProgram;

    void setColor(float r, float g, float b, float a = 1);
    void setTexScaling(float s);

    void setPosition(float x, float y, float z);
    void setPosition(QVector3D & position);

    void setScale(float s);
    void setScale(float x, float y, float z);
    void setScale(QVector3D & scale);

    void setRotation(QVector3D & axis, float angle);
    void setRotation(QQuaternion & rotation);


    void uppdateTransform();

    QVector3D getPosition(void);
    Model* getModel(void);

private:

    void initVariables(void);

    Model* model;
    QVector<GLuint> textures;

    QVector3D position;
    QVector3D scale;
    QQuaternion rotation;

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

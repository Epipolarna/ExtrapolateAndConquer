#ifndef MODEL_HPP
#define MODEL_HPP

#include <QVector>
#include <QVector3D>
#include <QGLShaderProgram>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "glm.h"

namespace graphics {

class ModelTriangle
{
public:
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector3D> texcoords;
};

class ModelGroup
{
public:
    // Old crap
    QVector<ModelTriangle> triangles;

    // New shit
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector3D> texCoords;
    QVector<GLuint> indices;
};

class Model
{
public:
    Model();
    Model(QString filename);
    void load(QString filename);
    void uploadToGPU();

    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, NBO, TBO, IBO, CBO, numberOfVertices, numberOfIndices;

private:
    QGLContext* context;

public:

    // functions

    //NOT IMPLEMENTED, probably best to remove
    //void draw(QGLShaderProgram& program, QMatrix4x4 vMatrix, QMatrix4x4 pMatrix, GLuint tex);
//private:
    // variables
    GLMmodel *model;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<ModelGroup> groups;
    GLuint texture;
    QMatrix4x4 mMatrix;
    QGLShaderProgram program;
};

}

#endif // MODEL_HPP

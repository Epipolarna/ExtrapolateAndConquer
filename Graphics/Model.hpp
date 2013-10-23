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
    QVector<ModelTriangle> triangles;
};

class Model
{
public:
    Model();
    Model(QString filename, QOpenGLVertexArrayObject* VAO);

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer VBO, NBO, TBO, IBO, CBO, numberOfVertices, numberOfIndices;

private:
    void load(QString filename);
    void uploadToGPU();

    QGLContext* context;

public:

    // functions

    void draw(QGLShaderProgram& program, QMatrix4x4 vMatrix, QMatrix4x4 pMatrix, GLuint tex);
//private:
    // variables
    GLMmodel *model;
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<ModelGroup> groups;
    GLuint texture;
    QMatrix4x4 mMatrix;
    QGLShaderProgram program;
    int vertexAttr;
    int normalAttr;
    int mvpMatUniform;
    int mMatUniform;
    int vMatUniform;
    int pMatUniform;
    int texCoordAttr;
    int textureUniform;
};

}

#endif // MODEL_HPP

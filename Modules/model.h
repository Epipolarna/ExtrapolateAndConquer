#ifndef MODEL_H
#define MODEL_H

#include <QObject>
//#include <QtOpenGL>
//#include <QGLShaderProgram>
#include <QPainter>
#include <QPaintEngine>

#include "glm.h"

class ModelTriangle
{
public:
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector3D> texcoords;
signals:

public slots:

};

class ModelGroup
{
public:
    QVector<ModelTriangle> triangles;
signals:

public slots:

};

class Model
{
public:
    Model();
    Model(QString filename);

    // functions
    void load(QString filename);
    void draw(QGLShaderProgram& program, QMatrix4x4 vMatrix, QMatrix4x4 pMatrix, GLuint tex);
    void setTexture(GLuint texture);
    bool setVertexShaderFile(QString filename);
    bool setFragmentShaderFile(QString filename);
    void setProgram();
    bool linkShaderProgram();
    void initShaderProgram(QGLShaderProgram& program);
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

#endif // MODEL_H

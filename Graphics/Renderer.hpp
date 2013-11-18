#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>
#include <vector>

#include "Model.hpp"
#include "Object.hpp"
#include "Camera.hpp"

class Renderer : public QOpenGLFunctions 
{
public:
    Renderer();

    void repaint(void);
    void drawObject(Object* o);
    void drawObjects(Model* model,QOpenGLShaderProgram* program,std::vector<QVector3D> positions);

    std::vector<Object*> renderList;

    QMatrix4x4 pMatrix;

    Object* skybox;
    Camera* camera;
    Object* water;
    Object* world;

    //Trees
    Model* treeModel;
    QOpenGLShaderProgram* treeShader;
    std::vector<QVector3D> treePositions;
private:
};

#endif // RENDERER_HPP

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QOpenGLFunctions_3_2_Core>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>
#include <vector>

#include "Model.hpp"
#include "Object.hpp"
#include "Camera.hpp"

class Renderer : public QOpenGLFunctions_3_2_Core
{
public:
    Renderer();

    void repaint(void);
    void drawObject(Object* o);
    void drawObjects(Model* model,QOpenGLShaderProgram* program,std::vector<QVector3D> positions, GLuint tex);

    void setSize(int width, int height);

    void initFBO();
    void useFBO(QGLFramebufferObject* FBO);

    std::vector<Object*> renderList;

    QMatrix4x4 pMatrix;

    Object* skybox;
    Camera* camera;
    Object* water;
    Object* world;
    Object* fboSquare;

    int width, height;

    //Trees
    Model* treeModel;
    QOpenGLShaderProgram* treeShader;
    QGLFramebufferObject *FBO1, *FBO2, *FBO3;
    GLuint fboID, fboColor, fboDepth;
    std::vector<QVector3D> treePositions;
    GLuint treeTexture;
private:
};

#endif // RENDERER_HPP

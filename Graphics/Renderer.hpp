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

struct FBO
{
    GLuint id, colorTex, depthTex;
};

class Renderer : public QOpenGLFunctions_3_2_Core
{
public:
    Renderer();

    void repaint(void);
    void drawObject(Object* o);
    void drawObjects(Model* model,QOpenGLShaderProgram* program,std::vector<QVector3D> positions, GLuint tex);

    void setSize(int width, int height);

    void initFBO(FBO *fbo);
    void useFBO(FBO *fbo);

    QOpenGLShaderProgram *depthProgram;

    std::vector<Object*> renderList;

    QMatrix4x4 pMatrix, lightSourceVMatrix;
    QVector3D lightPosition;

    Object* skybox;
    Camera* camera;
    Object* water;
    Object* world;
    Object* fboSquare;

    int width, height, shadowMapSize;

    //Trees
    Model* treeModel;
    QOpenGLShaderProgram* treeShader;
    FBO *fbo1, *fbo2, *fbo3;
    GLuint fboID, fboColorTex, fboDepth, fboDepthTex;
    std::vector<QVector3D> treePositions;
    GLuint treeTexture;
private:
};

#endif // RENDERER_HPP

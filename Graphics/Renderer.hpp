#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>
#include <vector>

#include "Model.hpp"
#include "Object.hpp"
#include "Camera.hpp"
#include "StaticObjectList.hpp"
#include "World.hpp"

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
    void drawInstanceObjects(StaticObjectList* statics, bool renderToDepthMap);

    void setSize(int width, int height);

    void initFBO(FBO *fbo);
    void useFBO(FBO *fbo);

    World* worldData;
    void calculateLightSourceMatrices();


    QOpenGLShaderProgram *depthProgram;
    QOpenGLShaderProgram *instanceDepthProgram;

    std::vector<Object*> renderList;

    QMatrix4x4 pMatrix;
    QMatrix4x4 pMatrixInv;
    QMatrix4x4 lightSourceVMatrix;
    QMatrix4x4 lightSourcePMatrix;
    QMatrix4x4 lightSourceVMatrixInv;
    QMatrix4x4 lightSourcePMatrixInv;
    std::vector<QVector4D> frustumCorners;

    QVector3D lightPosition;

    Object* skybox;
    Camera* camera;
    Object* water;
    Object* world;
    Object* fboSquare;

    int width, height, shadowMapSize;

    FBO *fbo1, *fbo2, *fbo3;
    GLuint fboID, fboColorTex, fboDepth, fboDepthTex;
    
private:
    static const int maxInstanceObjects = 50;

};

#endif // RENDERER_HPP

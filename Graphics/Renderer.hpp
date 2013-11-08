#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>
#include <vector>

#include "ModelLoader.hpp"
#include "Object.hpp"
#include "Camera.hpp"

class Renderer
{
public:
    Renderer();

    void render(void);

    QMatrix4x4 pMatrix;
    QMatrix4x4 cameraMatrix;
    graphics::Camera *cam;
    
    std::vector<graphics::Object> *objectList;
private:
};

#endif // RENDERER_HPP

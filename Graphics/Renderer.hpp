#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>
#include <vector>

#include "WorldGen.hpp"
#include "Model.hpp"
#include "Object.hpp"
#include "Camera.hpp"

class Renderer
{
public:
    Renderer();

    void repaint(void);

    std::vector<Object*> renderList;

    QMatrix4x4 pMatrix;

    Object* skybox;
    Camera* cam;

    Model* world;
private:
};

#endif // RENDERER_HPP

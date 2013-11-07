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

    std::vector<graphics::Object> *objectList;
private:
};

#endif // RENDERER_HPP

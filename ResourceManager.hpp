#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>
#include <QString>

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>

//needed for image format conversion function
#include <QGLWidget>

#include "Graphics/Object.hpp"
#include "Graphics/ModelLoader.hpp"


class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();
    
    ModelLoader* getModel(QString name);
    GLuint getTexture(QString name);
    QOpenGLShaderProgram* getShader(QString name);


    bool loadTexture(QString textureName,bool mipmap=false);
    bool loadModel(QString modelName);
    bool loadShader(QString shaderName);

private:
    const static QString resourcePath;
    const static QString modelPath;
    const static QString shaderPath;
    const static QString texturePath;


    std::map<QString,ModelLoader*> models;
    std::map<QString,GLuint> textures;
    std::map<QString,QOpenGLShaderProgram*> shaders;
};

#endif // RESOURCEMANAGER_HPP

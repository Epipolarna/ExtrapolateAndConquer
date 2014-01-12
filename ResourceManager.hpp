#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <map>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QString>

#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <QGLFramebufferObject>
#include <QOpenGLBuffer>

//needed for image format conversion function
#include <QGLWidget>

#include "Graphics/Object.hpp"
#include "Graphics/Model.hpp"


class ResourceManager : public QOpenGLFunctions
{
public:
    ResourceManager();
    ~ResourceManager();
    
    Model* getModel(const QString name);
    GLuint getTexture(const QString name);
    QOpenGLShaderProgram* getShader(const QString name);


    bool loadTexture(QString textureName,bool mipmap=false);
    bool loadTexture(QString textureName, QString textureFileName, bool mipmap);
    bool loadTextureAtlas(QString textureAtlasName, int atlasSize, bool mipmap=false);
    bool loadModel(QString modelName, bool unitize=false);
    bool loadShader(QString shaderName);

private:
    const static QString resourcePath;
    const static QString modelPath;
    const static QString shaderPath;
    const static QString texturePath;


    std::map<QString,Model*> models;
    std::map<QString,GLuint> textures;
    std::map<QString,QOpenGLShaderProgram*> shaders;
};

#endif // RESOURCEMANAGER_HPP

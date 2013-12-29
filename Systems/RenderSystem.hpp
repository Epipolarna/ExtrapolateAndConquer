#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include <QOpenGLFunctions_3_2_Core>

#include "../EntitySystem/EntitySystem.hpp"
#include "../Components/Components.hpp"

#include "../Graphics/World.hpp"
#include "../Graphics/Camera.hpp"
#include "../ResourceManager.hpp"

#ifndef FBO_H
#define FBO_H

struct FBO{
    GLuint id, colorTex, depthTex;
};

#endif


class RenderSystem : public System<Drawable, Components> , QOpenGLFunctions_3_2_Core{

public:
    void processStep(Drawable& object) override;
    void batch(void) override;
    
    void setWorld(World* w);
    void setCamera(Camera* c);
    void setResources(ResourceManager* rm);
private:

    //override the init function
    void setup(void) override;

    void drawWorld(void);
    void drawShadows(void);
    void drawObjects(void);


    //draw a single object using current shader etc
    void drawObject(Drawable& object);

    //reference to the program currently in use
    QOpenGLShaderProgram* currentProgram;

    //data for the viewport into the world...
    Camera* camera;

    //the world is kind of a special case in drawing,
    //so keep a reference to the world object/generator
    World* worldData;

    //we need a handle to the resouce manager for the shaders etc
    ResourceManager* resources;

    //list of all objects to draw
    std::vector<Drawable> objectList;

    void setMaterial(MaterialParameters pm);
    void setTexture(TextureParameters tm);
    void setMatrices(QMatrix4x4 mMatrix);

    //drawing helper functions
    void calculateLightSourceMatrix(void);
    void useFBO(FBO& fbo);
    void initFBO(FBO& fbo);
    static const std::string textureVariableNames[];
};

#endif
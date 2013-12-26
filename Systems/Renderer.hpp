#ifndef RENDER_SYSTEM_HPP
#define RENDER_SYSTEM_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"
#include "Graphics/World.hpp"

class RenderSystem : public System<Drawable, Components{
public:
    void processStep(Drawable& object) override;
    void batch(void) override;
    
    void setWorld(World* w);
    void setCamera(Camera* c);
private:
    void drawWorld(void);
    void drawShadows(void);
    void drawObjects(void);

    //data for the viewport into the world...
    Camera* camera;

    //the world is kind of a special case in drawing,
    //so keep a reference to the world object/generator    
    World* worldData;


    //objects to draw, weird tree like structure...
    std::map<QOpenGLShaderProgram*,std::vector<Drawable>> drawList;   
};

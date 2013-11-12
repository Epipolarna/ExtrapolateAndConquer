#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"


class SimplePhysicsSystem : public System<SimplePhysics, Components>
{
public:
    void processStep(SimplePhysics & physics) {
        physics.position += physics.velocity;
        Entity<Components> & e = getEntity(physics);
        std::cerr << std::to_string((long)&e)+" p: "+std::to_string(physics.position.x())+","+std::to_string(physics.position.y())+","+std::to_string(physics.position.z())+" v: "+std::to_string(physics.velocity.x())+","+std::to_string(physics.velocity.y())+","+std::to_string(physics.velocity.z());
    }
};

// Graphics require SimplePhysics
class GraphicsUpdateSystem : public System<Graphics, Components>
{
public:
    void processStep(Graphics & graphic) {
        Entity<Components> & e = getEntity(graphic);
        graphic.object->setPosition(e.get<SimplePhysics>().position.x(),
                                    e.get<SimplePhysics>().position.y(),
                                    e.get<SimplePhysics>().position.z());
    }
};

#endif

#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"

class SimplePhysicsSystem : public System<SimplePhysics, Components>
{
public:
    SimplePhysicsSystem(EntityManager<Components> & es):System<SimplePhysics, Components>::System(es) {}

    void processStep(SimplePhysics & physics) {
        physics.position += physics.velocity;
    }
};

// Graphics require SimplePhysics
class GraphicsUpdateSystem : public System<Graphics, Components>
{
public:
    GraphicsUpdateSystem(EntityManager<Components> & es):System<Graphics, Components>::System(es) {}

    void processStep(Graphics & graphic) {
        Entity<Components> & e = getEntity(graphic);
        graphic.setPosition(e.get<SimplePhysics>().position.x(),
                            e.get<SimplePhysics>().position.y(),
                            e.get<SimplePhysics>().position.z());
    }
};

#endif

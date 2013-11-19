#ifndef SYSTEMS_AI_HPP
#define SYSTEMS_AI_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"

// SimpleAI require SpherePhysics
class SimpleAiSystem : public System<SimpleAI, Components>
{
public:
    void processStep(SimpleAI & AI) override {
        Entity<Components> & e = getEntity(AI);
        SpherePhysics & physics = e.get<SpherePhysics>();

        physics.force += -physics.collisionVector.normalized()*10;
    }
};

#endif

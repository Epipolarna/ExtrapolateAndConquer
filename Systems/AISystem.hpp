#ifndef AI_SYSTEM_HPP
#define AI_SYSTEM_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"

class AISystem : public System<AI, Components>
{
public:
    void processStep(AI & ai) override {

    }
};

#endif

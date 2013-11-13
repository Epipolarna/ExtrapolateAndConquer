#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "EntitySystem/EntitySystem.hpp"

// Includes
#include "Graphics/Object.hpp"

// List all components
#define Components Name,SimplePhysics,Graphics

/*
 * Component prototype list
 ************************************************************/
struct Name;
struct SimplePhysics;
struct Graphics;


/*
 * Component list
 ************************************************************/
struct Name : public Component<> {
    const std::string getName() override { return "Name"; }

    std::string name;
};

struct SimplePhysics : public Component<> {
    const std::string getName() override { return "SimplePhysics"; }

    QVector3D position;
    QVector3D velocity;
};

struct Graphics : public Component<SimplePhysics>{
    const std::string getName() override { return "Graphics"; }

    graphics::Object* object = NULL;
};

#endif

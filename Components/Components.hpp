#ifndef COMPONENTS_HPP
#define COMPONENTS_HPP

#include "EntitySystem/EntitySystem.hpp"

// Includes
#include "Graphics/Object.hpp"
#include "Graphics/World.hpp"

// List all components
#define Components Name,SimplePhysics,Graphics,SpherePhysics,AI

/*
 * Component prototype list
 ************************************************************/
struct Name;
struct SimplePhysics;
struct SpherePhysics;
struct Graphics;
struct AI;


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

struct Graphics : public Component<SpherePhysics> {
    const std::string getName() override { return "Graphics"; }

    Object* object = NULL;
};

struct SpherePhysics : public Component<> {
    const std::string getName() override { return "SpherePhysics"; }

    float mass;                     // m 			(Positive)
    float elasticity;               // epsilon 		(Between 0 and 1)
    float friction;                 // friction     (Between 0 and 1)
    float momentOfInertia;          // Should be a matrix in the general case. For a sphere: 6/12*m*radius^2
    float gravitationalConstant;    // g            (Positive. in Sweden at sea level: 9.82)

    QVector3D position; 			// x = Integral( v, dt );
    QVector3D velocity; 			// v = P / m;
    QVector3D angularVelocity;      // w = L * I^-1
    QQuaternion angularVelocity2;   // w = L * I^-1

    QVector3D rotation;             // r = Integral( w, dt );
    QQuaternion rotation2;          // r = Integral( w, dt );


    QVector3D linearMomentum;       // P = Integral( F, dt );
    QVector3D force;				// F = ... external events ....

    QVector3D angularMomentum;      // L = Integral( torque, dt );
    QVector3D torque;               // T = ( -friction * m * g * (v - w.cross(rotationAxis)) ).cross(rotationAxis)

    float radius;	// Size of sphere - the object currently must be a sphere.

    QVector3D collisionVector;      // The sum of the vectors from all collisions

};

#include "aux_AI.hpp"
struct AI : public Component<SpherePhysics> {
    const std::string getName() override { return "AI"; }

    // Surrounding awareness
    float viewRange;
    ObjectsInSight objectsInSight;

    // Motion and Path finding
    bool hasLocationTarget;     // True if there is a location it is heading for.
    bool hasFarMotionPlan;      // True if a sparse long-distance plan leading to the targeted location has been created.
    QVector2D locationTarget;

};



#endif

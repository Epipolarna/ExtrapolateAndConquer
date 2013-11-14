#ifndef SYSTEMS_HPP
#define SYSTEMS_HPP

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"


class SimplePhysicsSystem : public System<SimplePhysics, Components>
{
public:
    void processStep(SimplePhysics & physics) override {
        physics.position += physics.velocity;
        Entity<Components> & e = getEntity(physics);
    }
};

// Graphics require SimplePhysics
class GraphicsUpdateSystem : public System<Graphics, Components>
{
public:
    void processStep(Graphics & graphic) override {
        Entity<Components> & e = getEntity(graphic);
        graphic.object->setPosition(e.get<SimplePhysics>().position.x(),
                                    e.get<SimplePhysics>().position.y(),
                                    e.get<SimplePhysics>().position.z());
    }
};

class SpherePhysicsSystem : public System<SpherePhysics, Components>
{
public:
    void processStep(SpherePhysics & physics) override {
        // Update rotation
        QVector3D rotationAxis = QVector3D(0, 1, 0);	// Currently not so generalized
        QVector3D actualVelocity = physics.velocity - QVector3D::crossProduct(physics.angularVelocity, rotationAxis);
        QVector3D frictionForce = -physics.friction * physics.mass * physics.gravitationalConstant * actualVelocity;	//gravitationalConstant = 9.82
        physics.torque = QVector3D::crossProduct(frictionForce, rotationAxis);

        // Update velocities
        physics.angularVelocity = physics.angularMomentum * (1.0/physics.momentOfInertia);
        physics.velocity = physics.linearMomentum / physics.mass;

        // Integrate variables
        physics.position += physics.velocity * dt;
        physics.rotation += physics.angularVelocity * dt;
        physics.linearMomentum += physics.force * dt;
        physics.angularMomentum += physics.torque * dt;

        //Reset force and torque
        physics.force = QVector3D(0,0,0);
        physics.torque = QVector3D(0,0,0);
    }
    void setTimeInterval(float dt) {
        this->dt = dt;
    }
private:
    float dt;
};

class SphereCollisionSystem : public System<SpherePhysics, Components>
{
public:
    void processStep(SpherePhysics & physics) override {
    }
    void batch() override  {
        std::vector<SpherePhysics> & components = getComponents();
        for(int i = 0; i < components.size()-1; i++) {
            for(int j = i+1; j < components.size(); j++) {
                SpherePhysics & A = components[i];
                SpherePhysics & B = components[j];
                QVector3D radialVector = A.position - B.position;
                float distance = radialVector.length();
                float addedDiameters = 2*(A.radius+B.radius);
                if(distance < addedDiameters)
                {
                    QVector3D velocityDifferance = A.velocity - B.velocity;
                    QVector3D radialVelocityDifferance = projectOn(velocityDifferance, radialVector);
                    float meanElasticity = (A.elasticity+B.elasticity)/2;
                    QVector3D linearMomentum_dt = radialVelocityDifferance*(meanElasticity + 1)/(1/A.mass + 1/B.mass);

                    A.position += radialVelocityDifferance * (2*A.radius - distance);
                    B.position -= radialVelocityDifferance * (2*B.radius - distance);
                    A.linearMomentum += linearMomentum_dt;
                    B.linearMomentum -= linearMomentum_dt;
                }

            }
        }
    }
private:
    static inline QVector3D projectOn(QVector3D & vector, QVector3D & basis) {
        return basis * QVector3D::dotProduct(vector, basis);
    }
};

#endif

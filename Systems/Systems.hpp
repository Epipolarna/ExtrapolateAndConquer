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

// Graphics require SpherePhysics
class GraphicsUpdateSystem : public System<Graphics, Components>
{
public:
    void processStep(Graphics & graphic) override {
        Entity<Components> & e = getEntity(graphic);

        //SimplePhysics & physics = e.get<SimplePhysics>();
        //graphic.object->setPosition(physics.position);

        SpherePhysics & physics = e.get<SpherePhysics>();
        graphic.object->setPosition(physics.position);
        graphic.object->setRotation(physics.rotation2);

        //qDebug() << "Position:" << physics.position;
        //qDebug() << "Rotation:" << physics.rotation2;
    }
};

class SpherePhysicsSystem : public System<SpherePhysics, Components>
{
public:
    void processStep(SpherePhysics & physics) override {
        // Update velocities
        physics.angularVelocity = physics.angularMomentum * (1.0/physics.momentOfInertia);
        physics.velocity = physics.linearMomentum / physics.mass;

        //... Quaternion hax (angular velocity & rotation)
        QVector3D w = physics.angularMomentum * (1.0/physics.momentOfInertia);
        QVector3D w_dt = w * dt;
        physics.angularVelocity2 = QQuaternion(std::cos(w_dt.length()/2), w_dt.length() > 0.0001 ? w_dt * std::sin(w_dt.length()/2)/w_dt.length() : w_dt);
        physics.angularVelocity2.normalize();
        physics.rotation2 = physics.rotation2*physics.angularVelocity2;
        physics.rotation2.normalize();

        // Integrate variables
        physics.position += physics.velocity * dt;
        physics.rotation += physics.angularVelocity * dt;
        physics.linearMomentum += physics.force * dt;
        physics.angularMomentum += physics.torque * dt;

        qDebug() << "torque:" << physics.force;
        qDebug() << "angularMomentum:" << physics.angularMomentum;

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

class SphereSphereCollisionSystem : public System<SpherePhysics, Components>
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


// TODO: work with the terrain (check if collision; calculate friction, update rotation, bounce)
class SphereTerrainCollisionSystem : public System<SpherePhysics, Components>
{
public:
    void processStep(SpherePhysics & physics) override {
        // Update rotation
        QVector3D rotationAxis = QVector3D(0, physics.radius, 0);	// Currently not so generalized
        QVector3D actualVelocity = physics.velocity - QVector3D::crossProduct(physics.angularVelocity, rotationAxis);
        QVector3D frictionForce = -physics.friction * physics.mass * physics.gravitationalConstant * actualVelocity;	//gravitationalConstant = 9.82
        physics.torque += QVector3D::crossProduct(frictionForce, rotationAxis);
    }
};

#endif

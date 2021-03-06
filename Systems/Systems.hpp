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

        // Slowly lower the kinectic energy...
        //physics.angularMomentum *= 0.999;
        //physics.linearMomentum *= 0.999;

        // Update velocities
        physics.angularVelocity = physics.angularMomentum * (1.0/physics.momentOfInertia);
        physics.velocity = physics.linearMomentum / physics.mass;

        // Quaternion hax (angular velocity & rotation integration)
        QVector3D w = physics.angularMomentum * (1.0/physics.momentOfInertia);
        QVector3D w_dt = w * dt;
        physics.angularVelocity2 = QQuaternion(std::cos(w_dt.length()/2), w_dt.length() > 0.0001 ? w_dt * std::sin(w_dt.length()/2)/w_dt.length() : w_dt);
        physics.angularVelocity2.normalize();
        physics.rotation2 = physics.rotation2*physics.angularVelocity2;
        physics.rotation2.normalize();

        // Integrate variables
        physics.position += physics.velocity * dt;
        //physics.rotation += physics.angularVelocity * dt;  // Not used, see quarternions above
        physics.linearMomentum += physics.force * dt;
        physics.angularMomentum += physics.torque * dt;

        // Reset force and torque
        physics.force = QVector3D(0,0,0);
        physics.torque = QVector3D(0,0,0);

        // apply gravity
        physics.force = QVector3D(0,-1,0) * physics.mass * physics.gravitationalConstant;
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
                QVector3D radialVector = (A.position - B.position).normalized();
                float distance = (A.position - B.position).length();
                float addedRadius = A.radius+B.radius;
                if(distance < addedRadius)
                {
                    QVector3D velocityA = A.linearMomentum/A.mass;
                    QVector3D velocityB = B.linearMomentum/B.mass;
                    QVector3D radialVelocityDifferance = projectOn(velocityB, radialVector) - projectOn(velocityA, radialVector);
                    float meanElasticity = (A.elasticity+B.elasticity)/2;
                    QVector3D linearMomentum_dt = radialVelocityDifferance*(meanElasticity + 1)/(1/A.mass + 1/B.mass);

                    A.position += radialVector * (addedRadius - distance);
                    B.position -= radialVector * (addedRadius - distance);
                    A.linearMomentum += linearMomentum_dt;
                    B.linearMomentum -= linearMomentum_dt;

                    // Collision induced rotation
                    QVector3D actualVelocityA = A.velocity - QVector3D::crossProduct(A.angularVelocity, radialVector);
                    QVector3D frictionForceA = -A.friction * A.mass * A.gravitationalConstant * actualVelocityA;
                    A.torque += QVector3D::crossProduct(frictionForceA, radialVector);

                    QVector3D actualVelocityB = B.velocity - QVector3D::crossProduct(B.angularVelocity, -radialVector);
                    QVector3D frictionForceB = -B.friction * B.mass * B.gravitationalConstant * actualVelocityB;
                    B.torque += QVector3D::crossProduct(frictionForceB, -radialVector);

                    // Collision induced friction force
                    //A.force += -(A.friction+B.friction)/2 * A.mass * A.gravitationalConstant * A.velocity;
                    //B.force += -(A.friction+B.friction)/2 * B.mass * B.gravitationalConstant * B.velocity;

                    // Lower Kinectic energy due to collision
                    //A.angularMomentum *= 0.99;
                    //A.linearMomentum *= 0.99;
                    //B.angularMomentum *= 0.99;
                    //B.linearMomentum *= 0.99;
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

        if(isColliding(physics.position, physics.radius))
        {

            // Lose some kinetic energy
            //physics.angularMomentum *= 0.99;
            //physics.linearMomentum *= 0.99;

            // Impulse collision with a terrain that have a mass == infinity
            QVector3D radialVector = normal.normalized();//(physics.position - terrainImpactPoint).normalized();
            float distance = (physics.position - terrainImpactPoint).length();

            QVector3D velocity = physics.linearMomentum/physics.mass;
            QVector3D radialVelocity = -projectOn(velocity, radialVector.normalized());
            QVector3D linearMomentum_dt = radialVelocity*(physics.elasticity + 1)*physics.mass;

            physics.position += radialVector * std::abs(physics.radius - distance);
            physics.linearMomentum += linearMomentum_dt;

            // Normal force
            physics.force += QVector3D(0,normal.y(),0) * physics.mass * physics.gravitationalConstant;// * 0.99;

            // Collision induced rotation
            float friction = (physics.friction + terrainFriction)/2;
            QVector3D actualVelocity = physics.velocity - QVector3D::crossProduct(physics.angularVelocity, normal);
            QVector3D frictionForce = -friction * physics.mass * physics.gravitationalConstant * actualVelocity;	//gravitationalConstant = 9.82
            //physics.torque += QVector3D::crossProduct(frictionForce, normal);

            // Collision induced friction force
            //physics.force += -friction * physics.mass * physics.gravitationalConstant * physics.velocity.normalized() * 0.9999;

        }
    }
    void setHeightMap(cv::Mat heightMap) { this->heightMap = heightMap; }
    void setWorld(World* world) { this->world = world; }
private:
    cv::Mat heightMap;
    World* world;
    QVector3D normal;
    QVector3D terrainImpactPoint;
    float terrainFriction;

    bool isColliding(QVector3D & position, float radius) {

        float groundLevel = world->getHeight(position.x(), position.z());
        float sphereBottom = position.y()-radius;

        if( sphereBottom < groundLevel ){
            float y_diff = sphereBottom - groundLevel;
            // world->getNormal is ready to use
            normal = (y_diff+radius) * QVector3D(0, 1, 0);
            normal = world->getNormal(position.x(), position.z());
            terrainImpactPoint = QVector3D(position.x(), groundLevel, position.z());
            terrainFriction = 0.3;
            return true;
        }
        return false;
    }
    static inline QVector3D projectOn(QVector3D vector, QVector3D basis) {
        return basis * QVector3D::dotProduct(vector, basis);
    }
};

#include "AISystem.hpp"

#endif

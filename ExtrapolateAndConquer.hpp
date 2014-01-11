#ifndef EXTRAPOLATEANDCONQUER_HPP
#define EXTRAPOLATEANDCONQUER_HPP

#include <QApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include <QOpenGLContext>
#include <map>

#include "ResourceManager.hpp"

#include "Graphics/OpenGLWindow.hpp"
#include "Graphics/Renderer.hpp"
#include "Graphics/Object.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/World.hpp"

#include "EntitySystem/EntitySystem.hpp"
#include "Components/Components.hpp"
#include "Systems/Systems.hpp"


class ExtrapolateAndConquer : QObject
{
    Q_OBJECT

public:
    ExtrapolateAndConquer(int argc, char *argv[]);
    ~ExtrapolateAndConquer();

    void initialize(void);

    void generateNewWorld(int seed = -1);   // -1 : random

    int run();

    void setState(int state);

public slots:
    void loopBody();

public:

    ResourceManager* resourceManager;

    EntityManager<Components> entityManager;

    SimplePhysicsSystem simplePhysicsSystem;
    GraphicsUpdateSystem graphicsUpdateSystem;
    SpherePhysicsSystem spherePhysicsSystem;
    SphereSphereCollisionSystem sphereSphereCollisionSystem;
    SphereTerrainCollisionSystem sphereTerrainCollisionSystem;
    AISystem aiSystem;

    QApplication* application;
    QOpenGLContext* context;
    OpenGLWindow* openGLWindow;
    Camera* camera;
    World* world;

    QTimer* timer;
    QElapsedTimer* fpsMeter;
    qint64 elapsedTime;

    Entity<Components> * e;
    cv::Mat hightMapOfChunk;

    std::map<int,SpherePhysics > previousState; //Used by backwards physics to restart fireballs when entering the volcano.
    bool vulcanActive;
    bool highResolutionTerrain;
    int state;
    int seed;

    void loadResources(void);

};

#endif // EXTRAPOLATEANDCONQUER_HPP

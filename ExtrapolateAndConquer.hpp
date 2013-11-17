#ifndef EXTRAPOLATEANDCONQUER_HPP
#define EXTRAPOLATEANDCONQUER_HPP

#include <QApplication>
#include <QTimer>
#include <QElapsedTimer>
#include <vector>
#include <QOpenGLContext>

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

    int run();
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

    void loadResources(void);
};

#endif // EXTRAPOLATEANDCONQUER_HPP

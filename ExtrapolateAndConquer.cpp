#include "ExtrapolateAndConquer.hpp"
#include <QDesktopWidget>
#include <QStyle>

ExtrapolateAndConquer::ExtrapolateAndConquer(int argc, char *argv[]){

    application = new QApplication(argc,argv);

    QSurfaceFormat format;
    format.setVersion(3,2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(32);

    context = new QOpenGLContext;
    context->setFormat(format);
    context->create();

    openGLWindow = new OpenGLWindow(context);
    openGLWindow->setHostApplication(application);
    openGLWindow->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                                  Qt::AlignCenter,
                                                  openGLWindow->size(),
                                                  application->desktop()->availableGeometry()));
    openGLWindow->show();

    camera = openGLWindow->getRenderer()->camera;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(loopBody()));
    fpsMeter = new QElapsedTimer;
}

ExtrapolateAndConquer::~ExtrapolateAndConquer(){
    //TODO destroy game
}

void ExtrapolateAndConquer::initialize(void){

    // ALX --------
    // Fog pos
    //camera->setPosition(QVector3D(0,17,0));
    //camera->setLookAtPoint(QVector3D(-1,17,-1));

    // ------------

    // Position 1
    //camera->setPosition(QVector3D(93.8781, 51.4692, -16.1341));
    //camera->setLookAtPoint(QVector3D(94.0255, 51.0261, -15.2498));
    // Position 2
    //camera->setPosition(QVector3D(-8.42683, 27.0048, 51.5548));
    //camera->setLookAtPoint(QVector3D(-7.60664, 26.7616, 52.0726));
    // Position 3
    //camera->setPosition(QVector3D(206.814, 13.8763, 56.0137));
    //camera->setLookAtPoint(QVector3D(205.873, 13.78, 56.3387));

    fpsMeter->start();
    openGLWindow->initialize();
    qDebug() << "OpenGL init: " << fpsMeter->elapsed() << "ms";

    fpsMeter->start();
    resourceManager = new ResourceManager;
    loadResources();
    qDebug() << "Resource Loading: " << fpsMeter->elapsed() << "ms";
    qDebug() << "asdfdsafdsafds ms";

    Renderer* renderer = openGLWindow->getRenderer();

    Object* o1 = new Object(resourceManager->getModel("tree0"),resourceManager->getShader("phong"));
    //renderer->renderList.push_back(o1);

    //the skybox needs to be specially added to the renderer
    Object* skybox = new Object(resourceManager->getModel("skybox"),resourceManager->getShader("skyboxShader"),resourceManager->getTexture("skybox1"));
    renderer->skybox = skybox;

    // FBO setup
    renderer->fboSquare = new Object(resourceManager->getModel("fboSquare"),resourceManager->getShader("fbo"),renderer->fbo1->colorTex);
    renderer->depthProgram = resourceManager->getShader("depth");
    renderer->instanceDepthProgram = resourceManager->getShader("instanceDepth");


    int nBalls = 40;
    qDebug() << "Initiating" << nBalls << "stones of each type (5 types).";

    qsrand(2);
    // -------- Stones --------------------------
    for(int i = 0; i < nBalls*5; i++){
        // Initialize entity
        e = &entityManager.createEntity();
        e->add<SpherePhysics>();
        e->add<Graphics>();

        SpherePhysics & physics = e->get<SpherePhysics>();
        physics.position = QVector3D(qrand()%150+10,-2,qrand()%150+65);
        physics.rotation2 = QQuaternion(1,0,0,0);
        physics.elasticity = 0.3;
        physics.friction = 0.3;
        physics.gravitationalConstant = 9.82;

        if(i < nBalls) {        // -------- Stone 1 -----------
            physics.mass = 100.0;
            physics.radius = 1.0;
            e->get<Graphics>().object = new Object(resourceManager->getModel("stone1"), resourceManager->getShader("phongTex"), resourceManager->getTexture("stone1"));
        } else
            if(i < nBalls*2) {  // -------- Stone 2 -----------
                physics.mass = 50.0;
                physics.radius = 0.45;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone2"), resourceManager->getShader("phongTex"), resourceManager->getTexture("stone2"));
        } else
            if(i < nBalls*3) {  // -------- Stone 3 -----------
                physics.mass = 30.0;
                physics.radius = 0.25;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone3"), resourceManager->getShader("phongTex"), resourceManager->getTexture("stone3"));
        } else
            if(i < nBalls*4) {  // -------- Stone 4 -----------
                physics.mass = 25.0;
                physics.radius = 0.2;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone4"), resourceManager->getShader("phongTex"), resourceManager->getTexture("stone4"));
        } else {                // -------- Stone 5 -----------
                physics.mass = 10.0;
                physics.radius = 0.15;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone5"), resourceManager->getShader("phongTex"), resourceManager->getTexture("stone5"));
        }
        physics.momentOfInertia = 6.0/12.0 * physics.mass * physics.radius * physics.radius;
        e->get<Graphics>().object->setScale(physics.radius);
        e->get<Graphics>().object->setShaderParameters(0.3, 0.5, 0.2, 100);

        renderer->drawObject(e->get<Graphics>().object);


    }


    // ------------- Generate world ------------------------
    fpsMeter->start();
    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/1.87;
    float gain = 0.66;

    //for each pixel, get the value
    float period = 400;
    float amplitude = 20;
    for (int i = 0; i < 16; i++)
    {
        octaves[i] = period;
        scales[i] = amplitude;

        period *= lacunarity;
        amplitude *= gain;
    }

    Model* worldModel;
    world = new World(resourceManager);
    renderer->lightPosition = world->lightPosition;

    Object* worldObject;

    uint seed = 1;  // -1 means random seed from current time
    int nOctaves = sizeof(octaves)/sizeof(float);
    worldModel = world->generateWorld(200,200,0.5f,octaves,scales,nOctaves, seed);
    hightMapOfChunk = world->heightMap;

    // Shadow map Matrices
    renderer->lightSourceVMatrix.setToIdentity();
    renderer->lightSourceVMatrix.lookAt(renderer->lightPosition, QVector3D(130,-100,130), QVector3D(0,1,0));
    renderer->lightSourceVMatrixInv = renderer->lightSourceVMatrix;
    renderer->lightSourceVMatrixInv.inverted();

    renderer->lightSourcePMatrix.setToIdentity();
    float nearPlane = world->lightPosition.length();
    float farPlane = (QVector3D(world->sizeX, 0, world->sizeZ) - world->lightPosition).length();
    //renderer->lightSourcePMatrix.perspective(60.0, 1, nearPlane, farPlane);
    renderer->lightSourcePMatrix.ortho(-100,100,-100,100,nearPlane,farPlane);
    renderer->lightSourcePMatrixInv = renderer->lightSourcePMatrix;
    renderer->lightSourcePMatrixInv.inverted();

    sphereTerrainCollisionSystem.setHeightMap((hightMapOfChunk*2*world->scaleFactor-world->scaleFactor));
    sphereTerrainCollisionSystem.setWorld(world);

    // Ground texturing
    QVector<GLuint> worldTextures = QVector<GLuint>();
    worldTextures.push_back(resourceManager->getTexture("sand2"));
    worldTextures.push_back(resourceManager->getTexture("grass2"));
    worldTextures.push_back(resourceManager->getTexture("rock1"));
    worldTextures.push_back(renderer->fbo1->depthTex);

    worldObject = new Object(worldModel, resourceManager->getShader("terrainShader"), worldTextures);
    worldObject->setShaderParameters(0.7f, 0.5f, 0.5f, 20);
    worldObject->setColor(85,196,48,255);
    //worldObject->setScale(2,0,2);
    worldObject->setTexScaling(10);

    renderer->world = worldObject;
    qDebug() << "Generating world: " << fpsMeter->elapsed() << "ms";

    // Water
    QVector<GLuint> ot = QVector<GLuint>();
    ot.push_back(resourceManager->getTexture("water"));
    ot.push_back(resourceManager->getTexture("skyboxWaterReflection"));
    ot.push_back(resourceManager->getTexture("waterNormalMap2"));
    ot.push_back(renderer->fbo1->depthTex);

    Object* ocean = new Object(resourceManager->getModel("hiResSquare"), resourceManager->getShader("oceanShader"),ot);

    ocean->setShaderParameters(0.4f, 0.4f, 1.0f, 50);
    ocean->setColor(59,58,99,170);
    //ocean->setScale(500,1,500);
    ocean->setTexScaling(2000);

    renderer->water = ocean;

    // Global influence map
    int resolution = 4;
    cv::Mat influenceMap = cv::Mat::zeros(world->sizeX*resolution, world->sizeZ*resolution, CV_8U);

    for(SpherePhysics & physics : entityManager.getComponents<SpherePhysics>()) {
        cv::circle(influenceMap, cv::Point2i(physics.position.x()*resolution, physics.position.z()*resolution), physics.radius, cv::Scalar(255), -1);
    }
    cv::dilate(influenceMap, influenceMap, cv::Mat::ones(3,3,CV_8U), cv::Point2i(-1,-1), 3);
    cv::GaussianBlur(influenceMap, influenceMap, cv::Size(9,9), 3);


    // Initialize systems
    graphicsUpdateSystem.initialize(entityManager);
    spherePhysicsSystem.initialize(entityManager);
    spherePhysicsSystem.setTimeInterval(0.01);  // Set dt. QTimer::interval() is in milliseconds
    sphereSphereCollisionSystem.initialize(entityManager);
    sphereTerrainCollisionSystem.initialize(entityManager);
    aiSystem.initialize(entityManager);
    aiSystem.setInfluenceMap(influenceMap);

    renderer->worldData = world;
    printf("all initialization done! \n");
}

void ExtrapolateAndConquer::loadResources(void){

    //FBO Square. Used to draw the scene on when it has been drawn to a FBO
    printf("loading shadowMapping data \n");
    resourceManager->loadShader("fbo");
    resourceManager->loadShader("depth");
    resourceManager->loadShader("instanceDepth");
    resourceManager->loadModel("fboSquare");

    //tree data
    printf("loading tree data \n");
    resourceManager->loadShader("phong");

    resourceManager->loadModel("tree0");

    resourceManager->loadModel("tree1a");
    resourceManager->loadModel("tree1b");
    resourceManager->loadModel("tree2a");
    resourceManager->loadModel("tree2b");
    resourceManager->loadModel("tree3a");
    resourceManager->loadModel("tree3b");
    resourceManager->loadModel("tree4a");
    resourceManager->loadModel("tree4b");

    resourceManager->loadTexture("tree1a");
    resourceManager->loadTexture("tree1b");
    resourceManager->loadTexture("tree2a");
    resourceManager->loadTexture("tree2b");
    resourceManager->loadTexture("tree3a");
    resourceManager->loadTexture("tree3b");
    resourceManager->loadTexture("tree4a");
    resourceManager->loadTexture("tree4b");

    resourceManager->loadModel("tree5");
    resourceManager->loadTexture("tree5");


    //Stone data
    printf("loading stone data \n");
    resourceManager->loadModel("stone1");
    resourceManager->loadModel("stone2");
    resourceManager->loadModel("stone3");
    resourceManager->loadModel("stone4");
    resourceManager->loadModel("stone5");

    resourceManager->loadTexture("stone1", true);
    resourceManager->loadTexture("stone2", true);
    resourceManager->loadTexture("stone3", true);
    resourceManager->loadTexture("stone4", true);
    resourceManager->loadTexture("stone5", true);


    printf("loading sphere data \n");
    resourceManager->loadShader("phongTex");
    resourceManager->loadModel("unitSphere10");
    resourceManager->loadTexture("sphere");

    //skybox data
    printf("loading skybox data \n");
    resourceManager->loadModel("skybox");
    resourceManager->loadTexture("skybox1");
    resourceManager->loadShader("skyboxShader");

    //ground data
    printf("loading ground data \n");
    //resourceManager->loadShader("terrainShader");
    resourceManager->loadShader("terrainShader");
    resourceManager->loadTexture("grass2", true);
    //resourceManager->loadTexture("sand0", true);
    resourceManager->loadTexture("sand2", true);
    resourceManager->loadTexture("rock1", true);
    //resourceManager->loadTexture("snow1", true);


    //water data
    printf("loading water data \n");
    //resourceManager->loadTexture("water", true);
    resourceManager->loadTexture("waterNormalMap2", true);
    //resourceManager->loadTexture("waterNormalMap1");
    //resourceManager->loadModel("unitSquare");
    resourceManager->loadModel("hiResSquare");
    resourceManager->loadShader("oceanShader");

    //resourceManager->loadTexture("skyTop", true);
    resourceManager->loadTexture("skyboxWaterReflection", true);

    resourceManager->loadShader("instance");

    //tree data
    resourceManager->loadTexture("bush");
    resourceManager->loadModel("bush",true);

    printf("all resources loaded! \n");
}

int ExtrapolateAndConquer::run(){

    initialize();
    timer->start(2);
    int returnCode = application->exec();
    return returnCode;
}

bool first = true;
void ExtrapolateAndConquer::loopBody(){
    timer->stop();
    float dt = fpsMeter->elapsed()/1000.0;
    fpsMeter->restart();

    spherePhysicsSystem.setTimeInterval(dt);
    openGLWindow->getRenderer()->setDt(dt);

    camera->updatePosition();
    world->getHeight(camera->position.x(), camera->position.z());
    world->getNormal(camera->position.x(), camera->position.z());


    SpherePhysics & physics = e->get<SpherePhysics>();
    //physics.force += QVector3D(0.1,0,0.2);

    // Respawn objects outside of the map
    for(SpherePhysics & sp : entityManager.getComponents<SpherePhysics>()) {
        if(sp.position.x() <= 0 || sp.position.z() <= 0 || sp.position.x() >= world->sizeX || sp.position.z() >= world->sizeZ || sp.position.y() < -8) {
            /*
            // Respawn over the ground
            sp.position = QVector3D(qrand()%150+10, qrand()%25+10, qrand()%150+65);

            // Reset physics. Comment out to get stones flying in your head :)
            sp.linearMomentum = QVector3D(0,0,0);
            sp.velocity = QVector3D(0,0,0);
            sp.angularMomentum = QVector3D(0,0,0);
            sp.angularVelocity2 = QQuaternion(1,0,0,0);
            */

            // Vulcano
            int m = sp.mass/2;
            int x = 72;
            int y = 22;
            int z = 161;
            sp.position = QVector3D(qrand()%3+x, y, qrand()%3+z);
            sp.velocity = QVector3D(0,0,0);
            sp.linearMomentum = QVector3D(qrand()%(30*m)-15*m,100*m,qrand()%(30*m)-15*m);
            sp.angularMomentum = QVector3D(qrand()%(2*m)-1*m,qrand()%(2*m)-1*m,qrand()%(2*m)-1*m);
            sp.angularVelocity2 = QQuaternion(1,0,0,0);
        }
    }

    // Run collision detection
    sphereSphereCollisionSystem.batch();    // Fetches all entities containing "Collision" components
    sphereTerrainCollisionSystem.batch();

    //qDebug() << "position" << physics.position << "| velocity " << physics.velocity << "|  linearMomentum" << physics.linearMomentum << "|  force" << physics.force;
    //qDebug() << "aVelocity" << physics.angularVelocity2 << "| angularMomentum" << physics.angularMomentum << "| torque" << physics.torque;
    //qDebug() << "---------------------------";

    // Run physics simulators
    spherePhysicsSystem.batch();

    // Run physics to graphics transfer of position/rotation
    graphicsUpdateSystem.batch();

    // Run AI
    aiSystem.batch();


    //make sure to update the gl widget...
    //graphicsWindow->centralWidget()->update();
    openGLWindow->update();

    //elapsedTime = fpsMeter->elapsed();
    //fpsMeter->restart();
    qDebug() << "FPS: " << 1/dt;
    timer->start();
}

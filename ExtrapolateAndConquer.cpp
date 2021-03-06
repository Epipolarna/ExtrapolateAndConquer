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

    // Demo settings
    vulcanActive = true;
    //state = -1;
    state = 7;
    seed = 3;
    highResolutionTerrain = false;

    // ALX --------
    // Fog pos
    //camera->setPosition(QVector3D(0,17,0));
    //camera->setLookAtPoint(QVector3D(-1,17,-1));

    // Shadow overview
    //camera->setPosition(QVector3D(-27.9003, 68.1128, 55.0443));
    //camera->setLookAtPoint(QVector3D(-27.1777, 68.1128, 55.5175));

    // Shadow close-up
    //camera->setPosition(QVector3D(60,9,106));
    //camera->setLookAtPoint(QVector3D(18.0365, -77.9033, 32.5638));

    // Shadow Acne
    //camera->setPosition(QVector3D(43.2642, 10.5588, 123.885));
    //camera->setLookAtPoint(QVector3D(-7.38341, -31.7233, 2.52191));

    // Peter Paning
    //camera->setPosition(QVector3D(40.4615, 7.07579, 116.255));
    //camera->setLookAtPoint(QVector3D(39.8829, 6.53198, 116.863));

    // ------------

    // Alpha-mip-mapping
    //camera->setPosition(QVector3D(45.9402, 5.14723, 113.396));
    //camera->setLookAtPoint(QVector3D(45.008, 5.08067, 113.041));

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


    int nBalls = 100;
    qDebug() << "Initiating" << nBalls << "stones of each type (5 types).";

    qsrand(3);
    // -------- Stones --------------------------
    for(int i = 0; i < nBalls*5; i++){
        // Initialize entity
        e = &entityManager.createEntity();
        e->add<SpherePhysics>();
        e->add<Graphics>();

        SpherePhysics & physics = e->get<SpherePhysics>();
        //physics.position = QVector3D(qrand()%150+10,-2,qrand()%150+65);
        physics.position = QVector3D(0,-10,0);
        physics.rotation2 = QQuaternion(1,0,0,0);
        physics.elasticity = 0.3;
        physics.friction = 1.0;
        physics.gravitationalConstant = 9.82;

        if(i < nBalls) {        // -------- Stone 1 -----------
            physics.mass = 100.0;
            physics.radius = 1.0;
            e->get<Graphics>().object = new Object(resourceManager->getModel("stone1"), resourceManager->getShader("phongTex"), resourceManager->getTexture("lava1"));
        } else
            if(i < nBalls*2) {  // -------- Stone 2 -----------
                physics.mass = 50.0;
                physics.radius = 0.45;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone2"), resourceManager->getShader("phongTex"), resourceManager->getTexture("lava1"));
        } else
            if(i < nBalls*3) {  // -------- Stone 3 -----------
                physics.mass = 30.0;
                physics.radius = 0.25;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone3"), resourceManager->getShader("phongTex"), resourceManager->getTexture("lava1"));
        } else
            if(i < nBalls*4) {  // -------- Stone 4 -----------
                physics.mass = 25.0;
                physics.radius = 0.2;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone4"), resourceManager->getShader("phongTex"), resourceManager->getTexture("lava1"));
        } else {                // -------- Stone 5 -----------
                physics.mass = 10.0;
                physics.radius = 0.15;
                e->get<Graphics>().object = new Object(resourceManager->getModel("stone5"), resourceManager->getShader("phongTex"), resourceManager->getTexture("lava1"));
        }
        physics.momentOfInertia = 6.0/12.0 * physics.mass * physics.radius * physics.radius;
        e->get<Graphics>().object->setScale(physics.radius);
        e->get<Graphics>().object->setShaderParameters(0.3, 0.5, 0.2, 100);

        renderer->drawObject(e->get<Graphics>().object);

    }


    world = new World(resourceManager);
    // ------------- Generate world ------------------------
    fpsMeter->start();
    float octaves[16];
    float scales[16];

    // 1.8715 or 2.1042
    float lacunarity = 1/2.1;   // Period reduction
    float gain = 0.52;          // Amplitude reduction

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
    renderer->lightPosition = world->lightPosition;

    Object* worldObject;

    // Nice worlds: 1, 13, ...
    uint seed = 447;  // -1 means random seed from current time
    int nOctaves = sizeof(octaves)/sizeof(float);
    float vertexDensity = 2.0f; // Determine the size & "sharpiness" of the world. Default: 0.5f
    worldModel = world->generateWorld(200,200,vertexDensity,octaves,scales,nOctaves, seed);
    hightMapOfChunk = world->heightMap;

    // Shadow map Matrices
    renderer->lightSourceVMatrix.setToIdentity();
    renderer->lightSourceVMatrix.lookAt(renderer->lightPosition, QVector3D(100,0,100), QVector3D(0,1,0));
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
    worldTextures.push_back(renderer->fbo2->depthTex);
    worldTextures.push_back(renderer->fbo3->depthTex);

    worldObject = new Object(worldModel, resourceManager->getShader("terrainShader"), worldTextures);
    worldObject->specialProgram1 = resourceManager->getShader("terrainShader");
    worldObject->specialProgram2 = resourceManager->getShader("terrainShader");
    worldObject->setShaderParameters(0.5f, 0.5f, 0.5f, 20);
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
    ot.push_back(renderer->fbo2->depthTex);
    ot.push_back(renderer->fbo3->depthTex);

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
    setState(state);
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

    resourceManager->loadTexture("tree1a", true);
    resourceManager->loadTexture("tree1b", true);
    resourceManager->loadTexture("tree2a", true);
    resourceManager->loadTexture("tree2b", true);
    resourceManager->loadTexture("tree3a", true);
    resourceManager->loadTexture("tree3b", true);
    resourceManager->loadTexture("tree4a", true);
    resourceManager->loadTexture("tree4b", true);

    // Bushes
    resourceManager->loadModel("tree5");
    resourceManager->loadModel("bush",true);
    resourceManager->loadTexture("tree5", false);
    resourceManager->loadTexture("bush", true);


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

    resourceManager->loadTexture("lava1", true);


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
    resourceManager->loadShader("terrainShader");
    resourceManager->loadShader("terrainShaderNoPCF");
    resourceManager->loadTexture("grass2", true);
    //resourceManager->loadTexture("sand0", true);
    resourceManager->loadTexture("sand2", true);
    resourceManager->loadTexture("rock1", true);
    //resourceManager->loadTexture("snow1", true);

    //water data
    printf("loading water data \n");
    resourceManager->loadTexture("water", true);
    resourceManager->loadTexture("waterNormalMap2", true);
    //resourceManager->loadTexture("waterNormalMap1");
    //resourceManager->loadModel("unitSquare");
    resourceManager->loadModel("hiResSquare");
    resourceManager->loadShader("oceanShader");

    //resourceManager->loadTexture("skyTop", true);
    resourceManager->loadTexture("skyboxWaterReflection", true);

    resourceManager->loadShader("instance");

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

    if(openGLWindow->isNewWorldRequested()) {
        qDebug() << " -> New World <-";
        generateNewWorld(++seed);
    }
    if(state != openGLWindow->currentState) {
        state = openGLWindow->currentState;
        setState(state);
        qDebug() << "new state:" << state;
        qDebug() << "-----------------------------";
    }
    if(openGLWindow->toggleTerrainResolution) {
        highResolutionTerrain = !highResolutionTerrain;
        qDebug() << "highResolutionTerrain:" << highResolutionTerrain;
        openGLWindow->toggleTerrainResolution = false;
        generateNewWorld(seed);
    } else
    if(openGLWindow->updateTerrainDensity) {
        qDebug() << "Terrain density:" << openGLWindow->terrainDensity;
        openGLWindow->updateTerrainDensity = false;
        generateNewWorld(seed);
    }
    if(openGLWindow->getRenderer()->isPCF){
        openGLWindow->getRenderer()->world->program = openGLWindow->getRenderer()->world->specialProgram1;
        //qDebug() << "Prog1";
    } else {
        openGLWindow->getRenderer()->world->program = openGLWindow->getRenderer()->world->specialProgram2;
        //qDebug() << "Prog2";
    }
    if(openGLWindow->backwardsPhysics) {
        dt *= -1;
    }

    spherePhysicsSystem.setTimeInterval(dt);
    openGLWindow->getRenderer()->setDt(dt);

    camera->updatePosition();
    world->getHeight(camera->position.x(), camera->position.z());
    world->getNormal(camera->position.x(), camera->position.z());


    SpherePhysics & physics = e->get<SpherePhysics>();
    //physics.force += QVector3D(0.1,0,0.2);

    // Respawn objects outside of the map
    if(vulcanActive)
    {
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
                if(openGLWindow->backwardsPhysics) {
                    if(previousState.find(sp.entityOwnerID) != previousState.end())
                        sp = previousState[sp.entityOwnerID];
                    else
                    {
                        int x = 0;
                        int y = -10;
                        int z = 0;
                        sp.position = QVector3D(qrand()%3+x, y, qrand()%3+z);
                        sp.velocity = QVector3D(0,0,0);
                        sp.linearMomentum = QVector3D(0,0,0);
                        sp.angularMomentum = QVector3D(0,0,0);
                        sp.angularVelocity2 = QQuaternion(1,0,0,0);
                    }
                } else {
                    previousState[sp.entityOwnerID] = sp;   // Used when backwardsPhysics

                    int m = sp.mass/2;
                    int x = (int)world->maxPosition.x();//72;
                    int z = (int)world->maxPosition.z();//161;
                    int y = (int)world->maxPosition.y();//world->getHeight(x,z)+1;//22;
                    sp.position = QVector3D(qrand()%3+x, y, qrand()%3+z);
                    sp.velocity = QVector3D(0,0,0);
                    sp.linearMomentum = QVector3D(qrand()%(30*m)-15*m,100*m,qrand()%(30*m)-15*m);
                    sp.angularMomentum = QVector3D(qrand()%(2*m)-1*m,qrand()%(2*m)-1*m,qrand()%(2*m)-1*m);
                    sp.angularVelocity2 = QQuaternion(1,0,0,0);
                }
            } else if(openGLWindow->backwardsPhysics) {
                bool isInsideVolcano = sp.position.y() < world->maxPosition.y() - 1 &&
                                       sp.position.x() > world->maxPosition.x() - 2 &&
                                       sp.position.x() < world->maxPosition.x() + 2 &&
                                       sp.position.z() > world->maxPosition.z() - 2 &&
                                       sp.position.z() < world->maxPosition.z() + 2;
                if(isInsideVolcano) {
                    //qDebug() << "entity" << sp.entityOwnerID << "is in volcano respawn box!";

                    if(previousState.find(sp.entityOwnerID) != previousState.end())
                        sp = previousState[sp.entityOwnerID];
                    else
                    {
                    /*
                    int x = (int)world->maxPosition.x()+10;//72;
                    int z = (int)world->maxPosition.z()+10;//161;
                    int y = world->getHeight(x,z)+2;
                    */
                    int x = 0;
                    int y = -10;
                    int z = 0;
                    sp.position = QVector3D(qrand()%3+x, y, qrand()%3+z);
                    sp.velocity = QVector3D(0,0,0);
                    sp.linearMomentum = QVector3D(0,0,0);
                    sp.angularMomentum = QVector3D(0,0,0);
                    sp.angularVelocity2 = QQuaternion(1,0,0,0);
                    }
                }
            }
        }
    }

    if(openGLWindow->activePhysics)
    {
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
    }

    //make sure to update the gl widget...
    //graphicsWindow->centralWidget()->update();
    openGLWindow->update();

    //elapsedTime = fpsMeter->elapsed();
    //fpsMeter->restart();
    //qDebug() << "FPS: " << 1/dt;
    timer->start();
}



void ExtrapolateAndConquer::generateNewWorld(int seed){
    Renderer* renderer = openGLWindow->getRenderer();

    // Generate terrain
    // -------------------------
    float octaves[16];
    float scales[16];

    float lacunarity;
    float gain;
    float vertexDensity;

    if(highResolutionTerrain) {
        lacunarity = 1/2.1;   // Period reduction
        gain = 0.52;          // Amplitude reduction
        vertexDensity = openGLWindow->terrainDensity;
    } else {
        //lacunarity = 1/1.87;
        //gain = 0.66;
        lacunarity = 1/2.1;   // Period reduction
        gain = 0.52;          // Amplitude reduction
        vertexDensity = 0.5f;
    }

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
    renderer->lightPosition = world->lightPosition;


    int nOctaves = sizeof(octaves)/sizeof(float);
    worldModel = world->generateWorld(200,200,vertexDensity,octaves,scales,nOctaves, seed);
    hightMapOfChunk = world->heightMap;

    //
    sphereTerrainCollisionSystem.setHeightMap((hightMapOfChunk*2*world->scaleFactor-world->scaleFactor));
    sphereTerrainCollisionSystem.setWorld(world);

    // Ground texturing
    QVector<GLuint> worldTextures = QVector<GLuint>();
    worldTextures.push_back(resourceManager->getTexture("sand2"));
    worldTextures.push_back(resourceManager->getTexture("grass2"));
    worldTextures.push_back(resourceManager->getTexture("rock1"));
    worldTextures.push_back(renderer->fbo1->depthTex);
    worldTextures.push_back(renderer->fbo2->depthTex);
    worldTextures.push_back(renderer->fbo3->depthTex);

    // Genrate World model with textures
    // ----------------------------------
    Object* worldObject;
    worldObject = new Object(worldModel, resourceManager->getShader("terrainShader"), worldTextures);
    worldObject->specialProgram1 = resourceManager->getShader("terrainShader");
    worldObject->specialProgram2 = resourceManager->getShader("terrainShader");
    worldObject->setShaderParameters(0.7f, 0.5f, 0.5f, 20);
    worldObject->setColor(85,196,48,255);
    //worldObject->setScale(2,0,2);
    worldObject->setTexScaling(10);

    renderer->world = worldObject;
    qDebug() << "Generating world: " << fpsMeter->elapsed() << "ms";

    renderer->worldData = world;
}

enum DemoState {
    WATER = 0,
    TERRAIN,
    BUSHES1,
    BUSHES2,
    TREES,
    FORESTS,
    VULCANO,
    VULCANO_ACTIVE,
    LAST_STATE
};

void ExtrapolateAndConquer::setState(int state){
    Renderer* renderer = openGLWindow->getRenderer();
    renderer->isRenderingTerrain = false;
    renderer->isRenderingTrees = false;
    renderer->isRenderingBalls = false;
    renderer->isRenderingShadows = false;
    world->hasVulcano = false;
    world->gaussForests = false;
    world->maxNumBush1 = 0;
    world->maxNumBush2 = 0;
    world->maxNumTrees = 0;
    vulcanActive = false;

    int numberOfTrees = 300;
    int numberOfBushes = 300;

    if(state > LAST_STATE) {
        openGLWindow->currentState = LAST_STATE;
        state = LAST_STATE;
    }

    switch(state){
        case WATER: //1) water and sky
            break;
        case TERRAIN:
            //land ugly textures
            renderer->isRenderingTerrain = true;
            renderer->isRenderingShadows = true;
            break;
       //CASE: TEXTURING (1-3?)
        case BUSHES1:  // 3.1) A few bushes
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;

            world->maxNumBush1 = 0.25*numberOfBushes;
            world->maxNumBush2 = 0.25*numberOfBushes/3;
            break;
        case BUSHES2:  // 3.2) Many bushes
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            break;
        case TREES:  // 3.2) Trees
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            world->maxNumTrees = numberOfTrees;
            break;
        case FORESTS:  // 3.2) Gaussian forests
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;
            world->gaussForests = true;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            world->maxNumTrees = numberOfTrees;
            break;
        //CASE: SHADOWS (1-4?)
        case VULCANO: // Place Vulcano
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;
            world->gaussForests = true;
            world->hasVulcano = true;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            world->maxNumTrees = numberOfTrees;
            break;
        case VULCANO_ACTIVE: // Activate Vulcano
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingShadows = true;
            world->gaussForests = true;
            world->hasVulcano = true;
            vulcanActive = true;
            renderer->isRenderingBalls = true;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            world->maxNumTrees = numberOfTrees;
            break;
        //CASE: broken worlds
        default:
        qDebug() << "!!! Default state (" << state << ")";
            //just make an new world with all the stuff enabled
            world->hasVulcano = true;
            renderer->isRenderingTerrain = true;
            renderer->isRenderingTrees = true;
            renderer->isRenderingBalls = true;
            renderer->isRenderingShadows = true;
            vulcanActive = true;
            seed += 1;

            world->maxNumBush1 = numberOfBushes;
            world->maxNumBush2 = numberOfBushes/3;
            world->maxNumTrees = numberOfTrees;

            break;
    }
    generateNewWorld(seed);

}

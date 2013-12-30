#include "World.hpp"
#include <iostream>
#include <QTime>

World::World( ResourceManager* resources){
    lightPosition = QVector3D(-100,100,-10);


    //do the setup for the trees, probably not really the right place for it tough...
    QVector<GLuint> treeTex1;
    QVector<GLuint> leafTex1;
    treeTex1.push_back(resources->getTexture("tree1a"));
    leafTex1.push_back(resources->getTexture("tree1b"));
    tree1 = new StaticObjectList(resources->getModel("tree1a"),treeTex1,resources->getShader("instance"));
    leaf1 = new StaticObjectList(resources->getModel("tree1b"),leafTex1,resources->getShader("instance"));


    QVector<GLuint> treeTex2;
    QVector<GLuint> leafTex2;
    treeTex2.push_back(resources->getTexture("tree2a"));
    leafTex2.push_back(resources->getTexture("tree2b"));
    tree2 = new StaticObjectList(resources->getModel("tree2a"),treeTex2,resources->getShader("instance"));
    leaf2 = new StaticObjectList(resources->getModel("tree2b"),leafTex2,resources->getShader("instance"));


    QVector<GLuint> treeTex3;
    QVector<GLuint> leafTex3;
    treeTex3.push_back(resources->getTexture("tree3a"));
    leafTex3.push_back(resources->getTexture("tree3b"));
    tree3 = new StaticObjectList(resources->getModel("tree3a"),treeTex3,resources->getShader("instance"));
    leaf3 = new StaticObjectList(resources->getModel("tree3b"),leafTex3,resources->getShader("instance"));
}

Model * World::generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves, uint seed){

    vertexDensity = _vertexDensity;
    sizeX = xRange;
    sizeZ = zRange;
    //lightPosition = QVector3D(-1000,500,-100);

    heightMap = cv::Mat(xRange*vertexDensity+1, zRange*vertexDensity+1, CV_32FC1);
    normalMap = cv::Mat(xRange*vertexDensity+1, zRange*vertexDensity+1, CV_32FC3);
    heightMapThresh = cv::Mat(xRange*vertexDensity+1, zRange*vertexDensity+1, CV_32FC1);

    int step = heightMap.cols;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> textures;
    QVector<unsigned int> indices;

    // find total scale to scale correctly when saved to cv::Mat
    scaleFactor = 0;
    for (int i = 0; i < nOctaves; i++){
            scaleFactor += yScales[i];
    }

    // -------------- Generate Vertices and Texture Coordinates -------------

    // Generate height map and texture coordinates
    if(seed < 0) {
        QTime now = QTime::currentTime();
        seed = now.msec();
    }
    qsrand(seed);
    float xRandomOffset = 45; //qrand() %256;
    float zRandomOffset = 107; //qrand() %256;
    /*
    float xRandomOffset = 45; //qrand() %256;
    float zRandomOffset = 107; //qrand() %256;
    qDebug() << "xRandomOffset" << xRandomOffset;
    qDebug() << "zRandomOffset" << zRandomOffset;
    */

    qDebug() << "xRandomOffset" << xRandomOffset;
    qDebug() << "zRandomOffset" << zRandomOffset;

    float y = 0;
    for (int x = 0; x <= xRange*vertexDensity; x++){
        for (int z = 0; z <= zRange*vertexDensity; z++){

            y = 0;
            if( x == 0 || z == 0 || x == xRange*vertexDensity || z == zRange*vertexDensity){
                y = -scaleFactor;
            } else {
                for(int i = 0; i < nOctaves; i++){
                    y += SimplexNoise1234::noise(x/octaves[i] + xRandomOffset, z/octaves[i] + zRandomOffset) * yScales[i];
                }
            }

            //vertices.push_back(QVector3D((float)x/vertexDensity, y, (float)z/vertexDensity));

            textures.push_back(QVector2D((float)x/(xRange*vertexDensity),(float)z/(zRange*vertexDensity)));

            heightMap.at<float>(x,z) = (y + scaleFactor) / (2*scaleFactor);
        }
    }

    // Manipulate height map

    cv::Mat gaussKernelX = cv::getGaussianKernel(xRange*vertexDensity+1, (xRange*vertexDensity+1)/3, CV_32FC1);
    float maxX = gaussKernelX.at<float>(floor((xRange*vertexDensity+1)/2));
    gaussKernelX /= maxX;
    cv::Mat gaussKernelZ = cv::getGaussianKernel(zRange*vertexDensity+1, (zRange*vertexDensity+1)/3, CV_32FC1);
    float maxZ = gaussKernelZ.at<float>(floor((zRange*vertexDensity+1)/2));
    gaussKernelZ /= maxZ;
    cv::Mat gaussKernel = gaussKernelX * gaussKernelZ.t();

    cv::threshold(gaussKernel, gaussKernel, 0.5, 1, 2);
    gaussKernel *= 2;
    //cv::imshow("gauss", gaussKernel);

    //cv::imshow("HeightMap Unmanipulated", heightMap);
    heightMap = heightMap.mul(gaussKernel);
    //cv::imshow("HeightMap manipulated", heightMap);

    // Push height map to VBO
    for (int x = 0; x <= xRange*vertexDensity; x++){
        for (int z = 0; z <= zRange*vertexDensity; z++){

            y = heightMap.at<float>(x,z);

            y = 2*y*scaleFactor - scaleFactor;

            vertices.push_back(QVector3D((float)x/vertexDensity, y, (float)z/vertexDensity));
        }
    }

    // -------------- Generate Faces ----------------------------

    // Tie vertices together. openGL indexing starts at 0 tydligen..
    for (int x = 1; x <= xRange*vertexDensity; x++){
        for (int z = 1; z <= zRange*vertexDensity; z++){

            // First triangle
            indices.push_back( (x-1)*step + z-1);  // 0
            indices.push_back( x*step     + z  );  // 3
            indices.push_back( x*step     + z-1);  // 2

            // Second triangle
            indices.push_back( (x-1)*step + z-1);  // 0
            indices.push_back( (x-1)*step + z  );  // 1
            indices.push_back( x*step     + z  );  // 3
        }
    }

    float y0, y1, y2, y3, y4, y5, y6, y7, y8;
    QVector3D t1, t2, t3, t4, t5, t6, t7, t8;
    QVector3D normal(0,0,0);

    // -------------- Generate Normals ----------------------------

    // Add standard normal for first.
    for (int z = 0; z <= zRange*vertexDensity; z++){
        normals.push_back(QVector3D(0,1,0));
        normalMap.at<cv::Vec3f>(0,z) = cv::Vec3f (0,1,0);;
    }

    for (int x = 1; x < xRange*vertexDensity; x++){
        normals.push_back(QVector3D(0,1,0));
        normalMap.at<cv::Vec3f>(x,0) = cv::Vec3f (0,1,0);

        for (int z = 1; z < zRange*vertexDensity; z++){

            y0 = heightMap.at<float>(x,z);
            y1 = heightMap.at<float>(x-1,z-1);
            y2 = heightMap.at<float>(x,z-1);
            y3 = heightMap.at<float>(x+1,z-1);
            y4 = heightMap.at<float>(x+1,z);
            y5 = heightMap.at<float>(x+1,z+1);
            y6 = heightMap.at<float>(x,z+1);
            y7 = heightMap.at<float>(x-1,z+1);
            y8 = heightMap.at<float>(x-1,z);

            // Scale & bias
            y0 = 2*y0*scaleFactor - scaleFactor;
            y1 = 2*y1*scaleFactor - scaleFactor;
            y2 = 2*y2*scaleFactor - scaleFactor;
            y3 = 2*y3*scaleFactor - scaleFactor;
            y4 = 2*y4*scaleFactor - scaleFactor;
            y5 = 2*y5*scaleFactor - scaleFactor;
            y6 = 2*y6*scaleFactor - scaleFactor;
            y7 = 2*y7*scaleFactor - scaleFactor;
            y8 = 2*y8*scaleFactor - scaleFactor;

            float vertexStep = 1/vertexDensity;

            t1 = QVector3D(-vertexStep, y1-y0, -vertexStep);
            t2 = QVector3D( 0,          y2-y0, -vertexStep);
            t3 = QVector3D( vertexStep, y3-y0, -vertexStep);
            t4 = QVector3D( vertexStep, y4-y0,  0);
            t5 = QVector3D( vertexStep, y5-y0,  vertexStep);
            t6 = QVector3D( 0,          y6-y0,  vertexStep);
            t7 = QVector3D(-vertexStep, y7-y0,  vertexStep);
            t8 = QVector3D(-vertexStep, y8-y0,  0);

            normal += QVector3D::crossProduct(t2,t1);
            normal += QVector3D::crossProduct(t3,t2);
            normal += QVector3D::crossProduct(t4,t3);
            normal += QVector3D::crossProduct(t5,t4);
            normal += QVector3D::crossProduct(t6,t5);
            normal += QVector3D::crossProduct(t7,t6);
            normal += QVector3D::crossProduct(t8,t7);
            normal += QVector3D::crossProduct(t1,t8);

            normal.normalize();

            normals.push_back(normal);

            cv::Vec3f cvNormal(normal.x(), normal.y(), normal.z());
            normalMap.at<cv::Vec3f>(x,z) = cvNormal;
        }
        normals.push_back(QVector3D(0,1,0));
        normalMap.at<cv::Vec3f>(xRange*vertexDensity,0) = cv::Vec3f (0,1,0);
    }

    for (int z = 0; z <= zRange*vertexDensity; z++){
        normals.push_back(QVector3D(0,1,0));
        normalMap.at<cv::Vec3f>(0,z) = cv::Vec3f(0,1,0);
    }

    // -------------- Save and return ----------------------------

    Model* worldModel = new Model();

    worldModel->modelFromData(vertices,normals,textures,indices);

    placeTrees();

    return worldModel;
}

float World::getHeight(float x, float z)
{
    float y = 0;
    // Check if the current position is on the terrain patch
    if(x >= 0 && x < (sizeX-1) &&
       z >= 0 && z < (sizeZ-1) )
    {
        float xScaled = x*vertexDensity;
        float zScaled = z*vertexDensity;
        float xFloor = floorf(xScaled);
        float xCeil  = ceilf( xScaled);
        float zFloor = floorf(zScaled);
        float zCeil  = ceilf( zScaled);

        float y00 = heightMap.at<float>(xFloor,zFloor);
        float y01 = heightMap.at<float>(xFloor,zCeil);
        float y10 = heightMap.at<float>(xCeil,zFloor);
        float y11 = heightMap.at<float>(xCeil,zCeil);

        float y0 = (1-(xScaled-xFloor))*y00 + (xScaled-xFloor)*y10;
        float y1 = (1-(xScaled-xFloor))*y01 + (xScaled-xFloor)*y11;

        y = (1-(zScaled-zFloor))*y0 + (zScaled-zFloor)*y1;
        y = y * 2*scaleFactor - scaleFactor;
    }

    //qDebug() << "Ground level at current position: " << y;

    return y;
}

QVector3D World::getNormal(float x, float z)
{
    QVector3D normal(0,1,0);
    // Check if the current position is on the terrain patch
    if(x >= 0 && x < (sizeX-1) &&
       z >= 0 && z < (sizeZ-1) )
    {
        float xScaled = x*vertexDensity;
        float zScaled = z*vertexDensity;
        float xFloor = floorf(xScaled);
        float xCeil  = ceilf( xScaled);
        float zFloor = floorf(zScaled);
        float zCeil  = ceilf( zScaled);

        cv::Vec3f n00 = normalMap.at<cv::Vec3f>(xFloor,zFloor);
        cv::Vec3f n01 = normalMap.at<cv::Vec3f>(xFloor,zCeil);
        cv::Vec3f n10 = normalMap.at<cv::Vec3f>(xCeil,zFloor);
        cv::Vec3f n11 = normalMap.at<cv::Vec3f>(xCeil,zCeil);

        cv::Vec3f n0 = (1-(xScaled-xFloor))*n00 + (xScaled-xFloor)*n10;
        cv::Vec3f n1 = (1-(xScaled-xFloor))*n01 + (xScaled-xFloor)*n11;

        cv::Vec3f n = (1-(zScaled-zFloor))*n0 + (zScaled-zFloor)*n1;

        normal = QVector3D( n[0], n[1], n[2]);
        normal.normalize();
    }

    //qDebug() << "Ground normal at current position: " << normal;

    return normal;
}

std::vector<QVector2D> World::getForests(void){
    int max_num_forests = 3;
    int max_num_iters = 10000; //in case we get unlycky with the worldgen, stop placing trees
    cv::RNG rng;

    std::vector<QVector2D> forests;
    //observe! two termination conditions
    for(int i=0; i < max_num_iters && forests.size() < max_num_forests; ++i){
        float x = rng.uniform((float)0.0,(float)sizeX);
        float z = rng.uniform((float)0.0,(float)sizeZ);
        float h = getHeight(x,z);
        //TODO use normal information too...
        //QVector3D normal = getNormal(x,z);

        if(h > 0){
            forests.push_back(QVector2D(x,z));
        }
    }

    return forests;
}

void World::addTree(int type, QVector3D position){
    cv::RNG generator;
    float rotation = (float)generator.uniform(0,600) / 100.0; //roughly a random ammount of radians

    QQuaternion randomRotation = QQuaternion(rotation,0,1,0);
    randomRotation.normalize();

    switch(type){
        case 0:
            tree1->appendObject(position,randomRotation);
            leaf1->appendObject(position,randomRotation);
        break;
        case 1:
            tree2->appendObject(position,randomRotation);
            leaf2->appendObject(position,randomRotation);
        break;
        case 2:
            tree3->appendObject(position,randomRotation);
            leaf3->appendObject(position,randomRotation);
        default:
            printf("INVALID TREE TYPE ERROR ! \n");
            exit(0);
        break;
    }
}

void World::placeTrees(void){
    //make a list of possible forest areas
    std::vector<QVector2D> forests = getForests();
    //make forests in these areas
    cv::RNG gen;

    int maxNumTrees = 50;
    int maxNumTries = 1000;

    for(QVector2D& forest : forests){

        float x = forest.x();
        float z = forest.y();
        //tree models need to be alittlebit in the ground
        float y = getHeight(forest.x(),forest.y()) - 1.0;

        int numTrees = 0;
        for(int i = 0; i < maxNumTries && numTrees < maxNumTrees; ++i){
            float xOffest = gen.uniform((float)-50.0,(float)50.0);
            float zOffset = gen.uniform((float)-50.0,(float)50.0);

            float treeX = x + xOffest;
            float treeZ = z + zOffset;
            float treeY = getHeight(treeX,treeZ);

            int treeType = gen.uniform((int)0,(int)2);
            //just remove the trees placed in lousy positions
            if(treeY > 0){
                addTree(treeType,QVector3D(treeX,treeY,treeZ));
            }
            numTrees = numTrees + 1;
        }
    }
}
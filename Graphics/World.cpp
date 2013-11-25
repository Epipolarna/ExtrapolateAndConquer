#include "World.hpp"
#include <iostream>
#include <QTime>

World::World(){
    lightPosition = QVector3D(-100,50,-10);
}

Model * World::generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves){

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
    QTime now = QTime::currentTime();
    qsrand(now.msec());
    float xRandomOffset = qrand() %256;
    float zRandomOffset = qrand() %256;

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

    //generateTexture();

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


//TODO construct texture data in non-flipped way...
void World::uploadCVTexture(void){
    glGenTextures(1,&textureRef);
    glBindTexture(GL_TEXTURE_2D, textureRef);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    
    glTexImage2D(GL_TEXTURE_2D,       
                 0,                   
                 GL_RGBA,              
                 textureData.cols,    
                 textureData.rows,    
                 0,                   
                 GL_BGRA,              
                 GL_UNSIGNED_BYTE,    
                 textureData.ptr());  
    
    //glGenerateMipmap(GL_TEXTURE_2D);
}

void World::generateTexture(){
    
    textureData = cv::Mat((int)heightMap.rows*vertexDensity,(int)heightMap.cols*vertexDensity,CV_8UC4);

    for(int i=0; i < heightMap.rows;++i){
        for(int j=0; j < heightMap.cols;++j){
            float h = getHeight(i,j);
            if(h < 0){
                textureData.at<cv::Vec4b>(i,j)[0] = 255;
                textureData.at<cv::Vec4b>(i,j)[1] = 0;
                textureData.at<cv::Vec4b>(i,j)[2] = 0;
                textureData.at<cv::Vec4b>(i,j)[3] = 255;
            }else if(h < 100){
                textureData.at<cv::Vec4b>(i,j)[0] = 0;
                textureData.at<cv::Vec4b>(i,j)[1] = 255;
                textureData.at<cv::Vec4b>(i,j)[2] = 0;
                textureData.at<cv::Vec4b>(i,j)[3] = 255;
            }else{
                textureData.at<cv::Vec4b>(i,j)[0] = 0;
                textureData.at<cv::Vec4b>(i,j)[1] = 255;
                textureData.at<cv::Vec4b>(i,j)[2] = 255;
                textureData.at<cv::Vec4b>(i,j)[3] = 255;
            }
        }
    }
    //cv::flip(textureData,textureData,0);
    //uploadCVTexture();
}

std::vector<QVector3D> World::placeTrees(void){
    //hack to place trees on the world
    //FIXME there is nothing right with this function
    printf("placing trees! \n");
    std::srand(std::time(0));
    int count = 0;
    std::vector<QVector3D> plants = std::vector<QVector3D>();
    for(int i=0; i < heightMap.cols; ++i){
        for(int j=0; j < heightMap.rows; ++j){
            float h = getHeight(i,j);
            if(h > 0){
                int random = std::rand();
                if(random % 10 == 0){
                    plants.push_back(QVector3D(i,h,j));
                }
            }
        }
    }
    printf("placed %d trees \n",plants.size());
    return plants;
}

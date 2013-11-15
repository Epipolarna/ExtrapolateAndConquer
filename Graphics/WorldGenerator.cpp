#include "WorldGenerator.hpp"

WorldGenerator::WorldGenerator(){

}

Model * WorldGenerator::generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves){

    vertexDensity = _vertexDensity;
    sizeX = xRange;
    sizeZ = zRange;

    heightMap = cv::Mat(xRange*vertexDensity, zRange*vertexDensity, CV_32FC1);
    heightMapThresh = cv::Mat(xRange*vertexDensity, zRange*vertexDensity, CV_32FC1);


    int step = heightMap.cols;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<QVector2D> textures;
    QVector<unsigned int> indices;

    //qDebug() << "Number of octaves: " << nOctaves;

    float y = 0;

    // find total scale to scale correctly when saved to cv::Mat
    scaleFactor = 0;
    for (int i = 0; i < nOctaves; i++){
            scaleFactor += yScales[i];
    }

    // Generate height map and texture coordinates
    for (int x = 0; x < xRange*vertexDensity; x++){
        for (int z = 0; z < zRange*vertexDensity; z++){

            y = 0;
            for(int i = 0; i < nOctaves; i++){
                y += SimplexNoise1234::noise(x/octaves[i], z/octaves[i]) * yScales[i];
            }

            vertices.push_back(QVector3D((float)x/vertexDensity, y, (float)z/vertexDensity));

            textures.push_back(QVector2D((float)x/(xRange*vertexDensity),(float)z/(zRange*vertexDensity)));

            heightMap.at<float>(x,z) = (y + scaleFactor) / (2*scaleFactor);
        }
    }

    //cv::imshow("heightMap", heightMap);
    cv::threshold(heightMap, heightMapThresh, 0.5, 1, 1);
    //cv::imshow("heightMapThresh", heightMapThresh);

    //qDebug() << "step: " << step;

    // Tie vertices together. openGL indexing starts at 0 tydligen..
    for (int x = 1; x < xRange*vertexDensity; x++){
        for (int z = 1; z < zRange*vertexDensity; z++){

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

    float y00, y01, y10;
    QVector3D tangent1, tangent2;
    QVector3D normal;

    // Calculate normals
    for (int z = 0; z < zRange*vertexDensity; z++){
        normals.push_back(QVector3D(0,1,0));
    }

    for (int x = 1; x < xRange*vertexDensity; x++){
        normals.push_back(QVector3D(0,1,0));
        for (int z = 1; z < zRange*vertexDensity; z++){

            y00 = heightMap.at<float>(x-1,z-1);
            y01 = heightMap.at<float>(x-1,z);
            y10 = heightMap.at<float>(x,z-1);

            tangent1 = QVector3D(1, y10-y00, 0);
            tangent2 = QVector3D(0, y01-y00, 1);

            normal = normal.crossProduct(tangent2, tangent1);
            normal.normalize();

            normals.push_back(normal);
        }
    }

    Model* worldModel = new Model();

    worldModel->modelFromData(vertices,normals,textures,indices);

    return worldModel;
}

float WorldGenerator::getHeight(float x, float z)
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

    qDebug() << "Ground level at current camera position: " << y;

    return y;
}

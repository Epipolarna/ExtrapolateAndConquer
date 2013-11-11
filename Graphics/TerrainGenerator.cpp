#include "TerrainGenerator.hpp"

#include <QDebug>
#include "simplexnoise1234.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

TerrainGenerator::TerrainGenerator()
{
}

graphics::Model *TerrainGenerator::simplexTerrain(float xRange, float zRange, float xPeriod, float zPeriod, float yScale)
{
    graphics::Model* model = new graphics::Model;
    graphics::ModelGroup group;
    cv::Mat image(xRange, zRange, CV_32FC1);

    float y00, y01, y10, y11;

    for (int x = 1; x < xRange; x++){
        for (int z = 1; z < zRange; z++){

            y00 =  SimplexNoise1234::noise((x-1)/xPeriod, (z-1)/zPeriod) * yScale;
            y01 =  SimplexNoise1234::noise((x-1)/xPeriod,  z   /zPeriod) * yScale;
            y10 =  SimplexNoise1234::noise( x   /xPeriod, (z-1)/zPeriod) * yScale;
            y11 =  SimplexNoise1234::noise( x   /xPeriod,  z   /zPeriod) * yScale;

            // Remap to [0, 1] and save to cv::Mat
            image.at<float>(x,z) = (y11 + yScale)/(2*yScale);

            //qDebug() << y00 << " " << y01 << " " << y10 << " " << y11;

            // First Triangle
            group.vertices.push_back(QVector3D(x-1, y00, z-1));
            group.vertices.push_back(QVector3D(x, y11, z));
            group.vertices.push_back(QVector3D(x, y10, z-1));

            // Second Triangle
            group.vertices.push_back(QVector3D(x-1, y00, z-1));
            group.vertices.push_back(QVector3D(x-1, y01, z));
            group.vertices.push_back(QVector3D(x, y11, z));
        }
    }

    model->groups.push_back(group);
    model->uploadToGPU();

    //cv::imshow("Terrain", image);

    return model;
}

graphics::Model *TerrainGenerator::simplexTerrain2(float xRange, float zRange, float vertexDensity, float octaves[], float yScales[], int nOctaves)
{
    graphics::Model* model = new graphics::Model;
    graphics::ModelGroup group;
    cv::Mat heightMap(xRange*vertexDensity, zRange*vertexDensity, CV_32FC1);
    cv::Mat heightMapThresh(xRange*vertexDensity, zRange*vertexDensity, CV_32FC1);
    int step = heightMap.cols;

    qDebug() << "Number of octaves: " << nOctaves;

    float y = 0;

    // find max scale to scale correctly when saved to cv::Mat
    float maxScale = 0;
    for (int i = 0; i < nOctaves; i++){
        if (yScales[i] > maxScale){
            maxScale = yScales[i];
        }
    }

    qDebug() << "MaxScale: " << maxScale;

    // Generate height map and texture coordinates
    for (int x = 0; x < xRange*vertexDensity; x++){
        for (int z = 0; z < zRange*vertexDensity; z++){

            y = 0;
            for(int i = 0; i < nOctaves; i++){
                y += SimplexNoise1234::noise(x/octaves[i], z/octaves[i]) * yScales[i];
            }

            group.vertices.push_back(QVector3D((float)x/vertexDensity, y, (float)z/vertexDensity));

            heightMap.at<float>(x,z) = (y + maxScale) / (2*maxScale);
        }
    }

    //cv::imshow("heightMap", heightMap);
    cv::threshold(heightMap, heightMapThresh, 0.5, 1, 1);
    //cv::imshow("heightMapThresh", heightMapThresh);

    qDebug() << "step: " << step;

    // Tie vertices together. openGL indexing starts at 0 tydligen..
    for (int x = 1; x < xRange*vertexDensity; x++){
        for (int z = 1; z < zRange*vertexDensity; z++){

            // First triangle
            group.indices.push_back( (x-1)*step + z-1);  // 0
            group.indices.push_back( x*step     + z  );  // 3
            group.indices.push_back( x*step     + z-1);  // 2

            // Second triangle
            group.indices.push_back( (x-1)*step + z-1);  // 0
            group.indices.push_back( (x-1)*step + z  );  // 1
            group.indices.push_back( x*step     + z  );  // 3
        }
    }

    float y00, y01, y10;
    QVector3D tangent1, tangent2;
    QVector3D normal;

    // Calculate normals
    for (int z = 0; z < zRange*vertexDensity; z++){
        group.normals.push_back(QVector3D(0,1,0));
    }

    for (int x = 1; x < xRange*vertexDensity; x++){
        group.normals.push_back(QVector3D(0,1,0));
        for (int z = 1; z < zRange*vertexDensity; z++){

            y00 = heightMap.at<float>(x-1,z-1);
            y01 = heightMap.at<float>(x-1,z);
            y10 = heightMap.at<float>(x,z-1);

            tangent1 = QVector3D(1, y10-y00, 0);
            tangent2 = QVector3D(0, y01-y00, 1);

            normal = normal.crossProduct(tangent2, tangent1);
            normal.normalize();

            group.normals.push_back(normal);
        }
    }

    model->groups.push_back(group);
    model->uploadToGPU();

    return model;
}

#include "TerrainGenerator.hpp"

#include "simplexnoise1234.h"

TerrainGenerator::TerrainGenerator()
{
}

graphics::Model *TerrainGenerator::simplexTerrain(float xRange, float zRange, float xPeriod, float zPeriod, float yScale)
{
    graphics::Model* model = new graphics::Model;
    graphics::ModelGroup group;

    float y00, y01, y10, y11;

    for (int x = 1; x < xRange; x++){
        for (int z = 1; z < zRange; z++){

            y00 =  SimplexNoise1234::noise((x-1)/xPeriod, (z-1)/zPeriod) * yScale;
            y01 =  SimplexNoise1234::noise((x-1)/xPeriod,  z   /zPeriod) * yScale;
            y10 =  SimplexNoise1234::noise( x   /xPeriod, (z-1)/zPeriod) * yScale;
            y11 =  SimplexNoise1234::noise( x   /xPeriod,  z   /zPeriod) * yScale;

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

    return model;
}

#ifndef TERRAINGENERATOR_HPP
#define TERRAINGENERATOR_HPP

#include "Model.hpp"

class TerrainGenerator
{
public:
    TerrainGenerator();

    static graphics::Model* simplexTerrain(float xRange, float zRange, float xPeriod = 1, float zPeriod = 1, float yScale = 1);
};

#endif // TERRAINGENERATOR_HPP

#ifndef TERRAINGENERATOR_HPP
#define TERRAINGENERATOR_HPP

#include "Model.hpp"

class TerrainGenerator
{
public:
    TerrainGenerator();

    static graphics::Model* simplexTerrain(float xRange, float zRange, float xPeriod = 1, float zPeriod = 1, float yScale = 1);

    /*!
     * \brief simplexTerrain2
     * \param xRange - Size of terrain in meters
     * \param zRange - Size of terrain in meters
     * \param vertexDensity - Vertices / meter
     * \param octaves - Period of each octave
     * \return
     */
    static graphics::Model* simplexTerrain2(float xRange, float zRange, float vertexDensity, float octaves[], float yScales[], int nOctaves);

};

#endif // TERRAINGENERATOR_HPP

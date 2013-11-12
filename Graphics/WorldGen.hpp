#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <vector>
#include <math.h>

#include <QVector3D>

#include "Model.hpp"
#include "simplexnoise1234.h"

class WorldGen{
	public:
		WorldGen();
		Model* generateWorld(float xRange, float zRange, float vertexDensity, float octaves[], float yScales[], int nOctaves);
};

#endif
#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "Model.hpp"

class WorldGen{
public:
	WorldGen();
	Model* generateWorld(float xRange, float zRange, float xPeriod = 1, float zPeriod = 1, float yScale = 1);
};

#endif
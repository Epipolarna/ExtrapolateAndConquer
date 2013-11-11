#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include "Model.hpp"

class WorldGen{
public:
	WorldGen();

	Model* generateWorld();
};

#endif
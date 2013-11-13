#ifndef WORLD_GEN_H
#define WORLD_GEN_H

#include <vector>
#include <math.h>

#include <QVector3D>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Model.hpp"
#include "simplexnoise1234.h"

class WorldGen{
	public:
		WorldGen();
		Model* generateWorld(float xRange, float zRange, float vertexDensity, float octaves[], float yScales[], int nOctaves);
};

#endif

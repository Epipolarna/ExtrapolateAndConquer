#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <vector>
#include <math.h>

#include <QVector3D>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Object.hpp"
#include "simplexnoise1234.h"

class WorldGenerator{
	public:
        WorldGenerator();
        Model* generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves);

        /*!
         * \brief getHeight at global position x and z.
         */
        float getHeight(float x, float z);

        cv::Mat heightMap;
        cv::Mat heightMapThresh;
        float scaleFactor;
        float vertexDensity;
        float sizeX;
        float sizeZ;
};

#endif

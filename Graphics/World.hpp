#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <math.h>

#include <QVector3D>
#include <QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Object.hpp"
#include "simplexnoise1234.h"

class World{
	public:
        World();
        Model* generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves);

        /*!
         * \brief getHeight at global position x and z.
         */
        float getHeight(float x, float z);
        QVector3D getNormal(float x, float z);

        cv::Mat heightMap;
        cv::Mat normalMap;
        cv::Mat heightMapThresh;
        float scaleFactor;
        float vertexDensity;
        float sizeX;
        float sizeZ;

        QVector3D lightPosition;
};

#endif

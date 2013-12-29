#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include <math.h>

#include <QVector3D>
#include <QDebug>
#include <QQuaternion>
#include <QVector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cstdlib>
#include <ctime>

#include "Object.hpp"
#include "simplexnoise1234.h"
#include "StaticObjectList.hpp"

#include "../ResourceManager.hpp"

class World{
	public:
        World( ResourceManager* resources);
        Model* generateWorld(void);
        StaticObjectList* getTrees(void);

        Model* getChunks(void);

        void setupWorldgen(void);

        /*!
         * \brief getHeight at global position x and z.
         */
        float getHeight(float x, float z);
        QVector3D getNormal(float x, float z);

        cv::Mat heightMap;
        cv::Mat normalMap;
        cv::Mat heightMapThresh;
        float scaleFactor;

        QVector3D lightPosition;

        GLuint textureRef;

        StaticObjectList* trees;
    private:

        Model* chunkCache;

        void placeTrees(void);
        void generateTexture(void);
        void uploadCVTexture(void);

        cv::Mat textureData;

        float xRange, zRange, vertexDensity;
        float *octaves, *yScales;
        int nOctaves;
};

#endif
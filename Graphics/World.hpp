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
        Model* generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves, uint seed = -1);

        
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

        GLuint textureRef;

        StaticObjectList* tree1;
        StaticObjectList* leaf1;

        StaticObjectList* tree2;
        StaticObjectList* leaf2;

        StaticObjectList* tree3;
        StaticObjectList* leaf3;

    private:

        void placeTrees(void);
        void generateTexture(void);
        void uploadCVTexture(void);

        cv::Mat textureData;
};

#endif

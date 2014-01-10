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
        Model* generateWorld(float xRange, float zRange, float _vertexDensity, float octaves[], float yScales[], int nOctaves, int seed = -1, bool spawnVulcan = true);

        
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

        QVector3D maxPosition;

        QVector3D lightPosition;

        GLuint textureRef;

        StaticObjectList* tree1;
        StaticObjectList* leaf1;

        StaticObjectList* tree2;
        StaticObjectList* leaf2;

        StaticObjectList* tree3;
        StaticObjectList* leaf3;

        StaticObjectList* bush1;
        StaticObjectList* bush2;

        int maxNumTrees;
        int maxNumBushes;
        bool hasVulcano;
        bool gaussForests;

    private:

        std::vector<QVector2D> getForests(void);
        void addTree(int type, QVector3D position, QVector3D scale = QVector3D(1,1,1));
        void placeTrees(void);
        void generateTexture(void);
        void uploadCVTexture(void);

        bool isTreePlacementOK(float treeX, float treeY, float treeZ, float sparsityStartHeight = 10000, float maxHeight = 10000);

        cv::Mat textureData;

        cv::RNG rGen;
};

#endif

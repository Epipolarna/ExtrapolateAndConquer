#include "WorldGen.hpp"

WorldGen::WorldGen(){

}

Model* WorldGen::generateWorld(float xRange, float zRange, float vertexDensity, float octaves[], float yScales[], int nOctaves){
	int xSize = (int)ceil(xRange*vertexDensity);
	int ySize = (int)ceil(zRange*vertexDensity);

	float heightmap[xSize][ySize];
    float y = 0;

	//get the max scale
	float maxScale = 0;
    for(int i = 0; i < nOctaves; i++){
        if (yScales[i] > maxScale){
            maxScale = yScales[i];
        }
    }

    std::vector<QVector3D> vertices;

	for (int x = 0; x < xRange*vertexDensity; x++){
    	for (int z = 0; z < zRange*vertexDensity; z++){
            y = 0;
            for(int i = 0; i < nOctaves; i++){
                y += SimplexNoise1234::noise(x/octaves[i], z/octaves[i]) * yScales[i];
            }
            vertices.push_back(QVector3D(x,y,z));
            heightmap[x][z] = (y + maxScale) / (2*maxScale);
        }
    }


    //TODO wtf?
    int step = xSize;
    std::vector<int> indices;
    for (int x = 1; x < xRange*vertexDensity; x++){
        for (int z = 1; z < zRange*vertexDensity; z++){

            // First triangle
            indices.push_back( (x-1)*step + z-1);  // 0
            indices.push_back( x*step     + z  );  // 3
            indices.push_back( x*step     + z-1);  // 2

            // Second triangle
            indices.push_back( (x-1)*step + z-1);  // 0
            indices.push_back( (x-1)*step + z  );  // 1
            indices.push_back( x*step     + z  );  // 3
        }
    }

    float y00, y01, y10;
    QVector3D tangent1, tangent2;
    QVector3D normal;
    std::vector<QVector3D> normals;

    // Calculate normals
    for (int z = 0; z < zRange*vertexDensity; z++){
        normals.push_back(QVector3D(0,1,0));
    }

    for (int x = 1; x < xRange*vertexDensity; x++){
        normals.push_back(QVector3D(0,1,0));
        for (int z = 1; z < zRange*vertexDensity; z++){

            y00 = heightmap[x-1][z-1];
            y01 = heightmap[x-1][z];
            y10 = heightmap[x][z-1];

            tangent1 = QVector3D(1, y10-y00, 0);
            tangent2 = QVector3D(0, y01-y00, 1);

            normal = normal.crossProduct(tangent2, tangent1);
            normal.normalize();

            normals.push_back(normal);
        }
    }

    //TODO add to model object and upload to GPU
    Model* worldModel = new Model();

    return worldModel;
}
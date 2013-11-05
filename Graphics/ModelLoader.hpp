#ifndef __MODEL_LOADER_H__
#define __MODEL_LOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QVector3D>
#include <QVector2D>
#include <QVector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QString>


class ModelLoader{
public:
	void loadModel(const QString filename);
	
	QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, NBO, TBO, IBO, CBO, numberOfVertices, numberOfIndices;


    //Model data on GPU
	QVector<QVector3D> vertex;
	QVector<QVector3D> normal;
	QVector<QVector2D> texture;
	QVector<unsigned int> index;

private:
	//file parsing bits
	void parseLine(const char *line, const size_t len);

	//state machine bits
	void parseVertex(const char *line, const size_t len);
	void parseFace(const char *line, const size_t len);
	//add vertex data
	void addTexture(const char* line, const size_t len);
	void addNormal(const char* line, const size_t len);
	void addVertex(const char* line, const size_t len);

	//make the GPU vertices
	void makeVertex(const int v_index,const int t_index,const int n_index);

	//check if a vertex alread is in the list
	bool vertexExists(const int v_index,const int t_index,const int n_index, const int i);

	//upload model to GPU
	void upload(void);

	//data read from file
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QVector<QVector2D> textures;

};

#endif
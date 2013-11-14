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
#include <QStringList>
#include <QFile>

class Model{
public:

	Model(void);

	void loadModel(const QString filename);
    void modelFromData(const QVector<QVector3D> vertices, const QVector<QVector3D> normals, const QVector<QVector2D> textures, const QVector<unsigned int> indices);
	
	QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer VBO, NBO, TBO, IBO;

    //Model data on GPU
	QVector<QVector3D> vertex;
	QVector<QVector3D> normal;
	QVector<QVector2D> texture;
	QVector<unsigned int> index;

private:
	//file parsing bits
	void parseLine(const QString line);
	
	bool readVertex(const QStringList data);
	bool readNormal(const QStringList data);
	bool readTexture(const QStringList data);
	bool readFace(const QStringList data);
	

	//make the GPU vertices
    void makeVertex(const QStringList data);

	//check if a vertex alread is in the list
    int vertexExists(const QVector3D v, const QVector3D n, const QVector2D t);

	//upload model to GPU
	void upload(void);

	void dumpData(void);

	int lineCounter;

	//data read from file
	QVector<QVector3D> vertices;
	QVector<QVector3D> normals;
	QVector<QVector2D> textures;
};

#endif

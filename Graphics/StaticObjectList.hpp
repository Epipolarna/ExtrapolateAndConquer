#ifndef STATICOBJECTLIST_HPP
#define STATICOBJECTLIST_HPP

#include <QOpenGLFunctions>
#include <QVector>
#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>

#include "Model.hpp"

class StaticObjectList : protected QOpenGLFunctions{
public:
	StaticObjectList( Model* m,QVector<GLuint> textures, QOpenGLShaderProgram* program);
	void appendObject( QVector3D position, QQuaternion rotation, QVector3D scale = QVector3D(1,1,1));
	void deleteObject( int index);
	int getNumObjects();
	
	QVector<QMatrix4x4> getMatrices(void);
	Model* getModel(void);
	QVector<GLuint> getTextures(void);
	QOpenGLShaderProgram* getProgram(void);

    void sortOnClosest(QVector3D center, int iterations = 1);

private:

	void updateMatrices(void);

	QVector<QVector3D> positions;
	QVector<QQuaternion> rotations;
	QVector<QMatrix4x4> mMatrices;
	QVector<QVector3D> scales;

	QOpenGLShaderProgram* program;

	bool matricesUpdated;
	Model* m;
	QVector<GLuint> textures;
	int numObjects;

    bool closerThan(QVector3D center, QVector3D pos1, QVector3D pos2);
};

#endif

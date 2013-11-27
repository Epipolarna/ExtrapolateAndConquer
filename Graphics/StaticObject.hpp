#ifndef STATIC_OBJECT_HPP
#define STATIC_OBJECT_HPP

#include <QVector3D>
#include <QQuaternion>

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

#include "Model.hpp"

class StaticObject{
public:
	StaticObject(const QVector3D pos,const Model* m);
	StaticObject(const QVector3D pos,const Model* m, const QQuaternion rot);
	QMatrix4x4 getMatrix(void);
private:
	void updateTransform(void);
	void initVariables(void);
	
	const QVector3D pos;
	const QQuaternion rot;
	const QVector3D scale;
	QMatrix4x4 mMatrix;
	
	const Model* m;

	//shader data
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float specularExponent;

    float texScaling;

    GLuint texture;
};

#endif
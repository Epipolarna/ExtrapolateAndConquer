#include "StaticObject.hpp"

StaticObject::StaticObject(const QVector3D pos,const Model* m) : pos(pos), m(m), rot(QQuaternion(1,0,0,0)){
	initVariables();
}

StaticObject::StaticObject(const QVector3D pos,const Model* m,const QQuaternion rot) : pos(pos), m(m), rot(rot){
	initVariables();
}

void StaticObject::updateTransform(void){
    mMatrix.setToIdentity();
    mMatrix.translate(pos);
    mMatrix.rotate(rot);
    mMatrix.scale(scale);
}

QMatrix4x4 StaticObject::getMatrix(void){
	return mMatrix;
}

void StaticObject::initVariables(void){
    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;

    texScaling = 1.0;
    updateTransform();
}
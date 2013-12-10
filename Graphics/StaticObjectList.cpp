#include "StaticObjectList.hpp"

StaticObjectList::StaticObjectList( Model* m,  QVector<GLuint> textures) : m(m), textures(textures) {
	matricesUpdated = false;
	numObjects = 0;
}

void StaticObjectList::appendObject( QVector3D position,  QQuaternion rotation, QVector3D scale){
	positions.push_back(position);
	rotations.push_back(rotation);
	scales.push_back(scale);
	numObjects = numObjects + 1;
	matricesUpdated = false;
}

void StaticObjectList::deleteObject( int index){
	positions.remove(index);
	rotations.remove(index);
	numObjects = numObjects - 1;
	matricesUpdated = false;
}

QVector<QMatrix4x4> StaticObjectList::getMatrices(void){
	if(matricesUpdated){
		return mMatrices;
	}else{
		updateMatrices();
		return mMatrices;
	}
}

void StaticObjectList::updateMatrices(void){
	for(int i=0; i < numObjects; ++i){
		QMatrix4x4 mMatrix;
		mMatrix.setToIdentity();
		mMatrix.translate(positions[i]);
		mMatrix.rotate(rotations[i]);
		mMatrix.scale(scales[i]);
	}
	matricesUpdated = true;
}
#ifndef GRAPHICS_UTILITIES_HPP
#define GRAPHICS_UTILITIES_HPP

#include <QVector3D>

struct MaterialParameters{
	float ambientCoeff;
	float diffuseCoeff;
	float speculaCoeff;
	float specularExponent;
};

struct TextureParameters{
	unsigned int texture[16];
	float scale[16];
};

struct LightParameters{
	QVector3D position;
};

#endif

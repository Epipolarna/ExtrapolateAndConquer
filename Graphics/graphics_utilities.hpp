#ifndef GRAPHICS_UTILITIES_HPP
#define GRAPHICS_UTILITIES_HPP

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

#endif

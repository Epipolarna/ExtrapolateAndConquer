#version 150

in vec3 exPosition;
in vec3 exNormal;
in vec2 exTexCoord;

uniform sampler2D tex0;
uniform float texScaling;

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

uniform mat4 vMatrix;

out vec4 outColor;

void main(void)
{
	vec2 scaledTexCoord = exTexCoord*texScaling;
	
	vec4 texel0 = texture(tex0, scaledTexCoord);	// Main Texture

	gl_FragDepth = 1 - texel0.a;
}

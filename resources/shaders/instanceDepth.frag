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
	
	// Phong
	vec3 cameraPosition = -transpose(mat3(vMatrix)) * vMatrix[3].xyz;

	vec3 lightPosition = vec3(-1000,500,-100);
	vec3 normal = normalize(exNormal);
	
	vec3 lightDirection = normalize(lightPosition - exPosition);
	
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    vec3 cameraDirection = normalize(cameraPosition - exPosition);
	
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	
	float shading = ambientCoeff + diffuseCoeff*diffuseComponent + specularCoeff*specularComponent;
	

	outColor = vec4(texel0.rgb*shading, texel0.a);
}

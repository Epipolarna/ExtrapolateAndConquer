#version 150

in vec3 exPosition;
in vec3 exNormal;
in vec2 exTexCoord;

uniform vec3 lightPosition;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform float texScaling;

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;
float specularComponent;

uniform float wave;	// [0 3.1415]
uniform float incr;	// [0 1]

uniform mat4 vMatrix;

uniform vec4 color;
uniform vec3 scale;

out vec4 outColor;

vec3 normalFromNormalMap(sampler2D normalMap)
{
	vec2 scaledTexCoord = exTexCoord*texScaling;
	scaledTexCoord[1] += incr;
	
	vec4 texel2 = texture(tex2, scaledTexCoord); 	// Normalmap
	vec3 normal = normalize(texel2.rbg *2-1);
	return normal;
}

float phongShading()
{
	// This function is a bit modifyed to support normalMap
	// and to set the global specularComponent used to blend water alpha
	vec3 cameraPosition = -transpose(mat3(vMatrix)) * vMatrix[3].xyz;
	
	vec3 normal = normalFromNormalMap(tex2); // Get normal from normalMap
	
	vec3 lightDirection = normalize(lightPosition - exPosition);
	vec3 cameraDirection = normalize(cameraPosition - exPosition);
	
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	
	float shading = ambientCoeff + diffuseCoeff*diffuseComponent + specularCoeff*specularComponent;
	
	return shading;
}

float fogBlending()
{
	float depth = length((vMatrix * vec4(exPosition,1)).xyz);
	float density = 0.005;
    const float e = 2.71828182845904523536028747135266249;
	
    float blendingFactor = pow(e, -pow(depth*density, 2));
	
    return blendingFactor;
}  

void main(void){
	
	float shading = phongShading();
	
	outColor = vec4(color.rgb*shading, color.a);
	outColor += vec4(1,1,1,1)*specularComponent;
	
	vec4 fogColor = vec4(0.8,0.8,0.8,1.0);
	outColor = mix(fogColor, outColor, fogBlending());
	//outColor = vec4(sin(wave), 0, 0, 1);
}

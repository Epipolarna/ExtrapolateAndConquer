#version 150

in vec3 exPosition;
in vec3 exNormal;
in vec2 exTexCoord;
in vec4 lightSpaceVertex;

uniform vec3 lightPosition;
uniform sampler2D tex0;
uniform sampler2D tex3;
uniform float texScaling;

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

uniform mat4 vMatrix;

uniform vec4 color;
uniform vec3 scale;

out vec4 outColor;

float phongShading()
{
	vec3 cameraPosition = -transpose(mat3(vMatrix)) * vMatrix[3].xyz;
	
	vec3 normal = normalize(exNormal);
	
	vec3 lightDirection = normalize(lightPosition - exPosition);
	vec3 cameraDirection = normalize(cameraPosition - exPosition);
	
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	
	float shading = diffuseCoeff*diffuseComponent + specularCoeff*specularComponent;
	
	return shading;
}

float fogBlending()
{
	float depth = length((vMatrix * vec4(exPosition,1)).xyz);
	float density = 0.005;
    const float e = 2.71828182845904523536028747135266249;
	
    float blendingFactor = pow(e, -pow(depth*density, 2.0));
	
	float height = 30;
	float heightCoeff = max(exPosition.y, height);
	heightCoeff -= height;
	heightCoeff /= 30;
	
	// Remove if high in the air
	
    return min(max(blendingFactor + heightCoeff, 0), 1);
}  
/*
float shadowTest(vec2 texcoods) {
	float shadow = texture(tex3, texcoods).r;
	float epsilon = 0.000001;
	if (shadow + epsilon < lightSpaceVertex.z) {
		return 0.5; // shadowed
	}
	return 1.0; // not shadowed
}
*/
int nShadows = 0;
float shadowCoeff = 0.5;

float shadowTest(vec2 texcoods, float kernelSize) {
	float shadow = 0;
	float depthComparison = 0;
	float epsilon = 0.001;
	//float epsilon = 0.0003;
	
	float texOffset = 0.7/(kernelSize*2048); // Motsvarar spridning på skuggan
	float shadowStep = 0.1;
	
	// 
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods).r;
	if(depthComparison > epsilon){
		shadow += 4;
	}
	
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(-texOffset,-texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 1;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(-texOffset,texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 1;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(texOffset,-texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 1;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(texOffset,texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 1;
	}
	
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(texOffset,0)).r;
	if(depthComparison > epsilon){
		shadow += 2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(-texOffset,0)).r;
	if(depthComparison > epsilon){
		shadow += 2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(0,texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(0,-texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 2;
	}
	
	// Normalize the kernel
	shadow /= 16;
	
	
	
	return (ambientCoeff + (1 - shadow)*(1-ambientCoeff));
}

void main(void)
{
	vec2 scaledTexCoord = exTexCoord*texScaling;
	vec4 texel0 = texture(tex0, scaledTexCoord);
	
	if(exPosition.y < 0){
		outColor = texel0*ambientCoeff*ambientCoeff;
	} else {
		int kernelSize = 2;
		
		outColor = texel0*(ambientCoeff + phongShading()*shadowTest(lightSpaceVertex.xy, kernelSize));
	}
	
	vec4 fogColor = vec4(0.8,0.8,0.8,1.0);
	outColor = mix(fogColor, outColor, fogBlending());
}

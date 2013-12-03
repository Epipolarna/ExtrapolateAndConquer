#version 150

in vec3 exPosition;
in vec3 exNormal;
in vec2 exTexCoord;
in vec4 lightSpaceVertex;

uniform vec3 lightPosition;
uniform sampler2D tex0;	// sand
uniform sampler2D tex1;	// grass
uniform sampler2D tex2; // rock
uniform sampler2D tex3;	// Shadow
uniform float texScaling;
	

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

uniform mat4 mMatrix;
uniform mat4 vMatrix;

uniform vec4 color;

out vec4 outColor;

float phongShading()
{
	vec3 normal = normalize(exNormal);
	
	// Diffuse
	vec3 lightDirection = normalize(lightPosition - exPosition);
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	
	// Specular
	/*
	vec3 cameraPosition = -transpose(mat3(vMatrix)) * vMatrix[3].xyz;
	vec3 cameraDirection = normalize(cameraPosition - exPosition);
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	*/
	
	//float shading = ambientCoeff + diffuseCoeff*diffuseComponent + specularCoeff*specularComponent;
	float shading = ambientCoeff + diffuseCoeff*diffuseComponent;
	
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

int nShadows = 0;
float shadowCoeff = 0.5;

float shadowTest(vec2 texcoods, int kernelSize) {
	float shadow = 0;
	float depthComparison = 0;
	float epsilon = 0.000001;
	
	float texOffset = 3.0/(kernelSize*1000); // Motsvarar spridning på skuggan
	
	for(int i = 0; i < kernelSize; i++){
		for(int j = 0; j < kernelSize; j++){
		
			depthComparison = lightSpaceVertex.z - texture(tex3, texcoods + vec2(texOffset*(i - kernelSize/2), texOffset*(j - kernelSize/2))).r;
			if(depthComparison > epsilon){
				shadow += 1.0 / pow(kernelSize,2);
				nShadows += 1;
			}
		}
	}
	
	return (ambientCoeff + (1 - shadow)*(1-ambientCoeff));
}

vec4 mixTextures(sampler2D texture1, sampler2D texture2, float x, float start, float stop, int exp)
{
	vec2 scaledTexCoord = exTexCoord*texScaling;
	return mix(texture(texture1, scaledTexCoord), texture(texture2, scaledTexCoord), pow((x-start)/(stop-start),exp));
}

vec4 blendTextures(sampler2D sand, sampler2D grass, sampler2D rock)
{
	vec2 scaledTexCoord = exTexCoord*texScaling;
	float height = exPosition.y;
	float horizontal = dot(exNormal, vec3(0,1,0));
				
	float grassStart = 0.1;
	float sandEnd = 2;
	float rockStart = 8;
	float grassEnd = 14;
		
	if(height < grassStart)
		return texture(sand, scaledTexCoord);
	if(height <= sandEnd)
		return mix(mixTextures(sand, grass, height, grassStart, sandEnd, 2),
		           mixTextures(sand, grass, height, grassStart, sandEnd, 1), 2*horizontal);
	if(height < rockStart)
		return texture(grass, scaledTexCoord);
	if(height <= grassEnd)
		return mix(mixTextures(grass, rock, height, rockStart, grassEnd, 1),
		           mixTextures(grass, rock, height, rockStart, grassEnd, 2), 2*horizontal);
	return texture(rock, scaledTexCoord);
}


void main(void){

	vec2 scaledTexCoord = exTexCoord*texScaling;
	vec4 texel3 = texture(tex3, exTexCoord);
	
	// Texture blending
	vec4 texel0 = blendTextures(tex0, tex1, tex2);

	/*
	float zBuffer = texel3.x;
    zBuffer = 2.0 * zBuffer - 1.0;
	float zNear = 0.1; 
	float zFar = 1000.0;
    float zNormalized = 2.0 * zNear * zFar / (zFar + zNear - zBuffer * (zFar - zNear));
	zNormalized = zNormalized/zFar;
	*/
	//outColor = phongShading()*texel0;
	//outColor = vec4(vec3(zNormalized), 1);
	
	if(exPosition.y < 0){ 
		// If under water
		outColor = texel0*ambientCoeff*ambientCoeff;
		
	} else { 
		// If over water
		int testKernelSize = 3;
		int kernelSize = 4;
		//shadowTest(lightSpaceVertex.xy, testKernelSize);
	
		outColor = texel0*phongShading()*shadowTest(lightSpaceVertex.xy, kernelSize);
	/*
		if(nShadows == 0){
			outColor = texel0*phongShading();
			//outColor = vec4(1,0,0,1);
		} else if(nShadows == pow(testKernelSize, 2)){
			outColor = texel0*ambientCoeff*ambientCoeff;
			//outColor = vec4(0,1,0,1);
		} else {
			outColor = texel0*phongShading()*shadowTest(lightSpaceVertex.xy, kernelSize);
			//outColor = vec4(0,0,1,1);
		}
		*/
	}
	
	vec4 fogColor = vec4(0.8,0.8,0.8,1.0);
	outColor = mix(fogColor, outColor, fogBlending());
	
	//outColor = vec4(vec3(zNormalized), 1);
}





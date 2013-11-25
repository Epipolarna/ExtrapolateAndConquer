#version 150

in vec3 exPosition;
in vec3 exNormal;
in vec2 exTexCoord;
in vec4 lightSpaceVertex;

uniform vec3 lightPosition;
uniform sampler2D tex0;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
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
	vec3 cameraPosition = -transpose(mat3(vMatrix)) * vMatrix[3].xyz;
	
	vec3 normal = normalize(exNormal);
	
	vec3 lightDirection = normalize(lightPosition - exPosition);
	vec3 cameraDirection = normalize(cameraPosition - exPosition);
	
	vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    
	float diffuseComponent = max(dot(normal, lightDirection), 0);
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);
	
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

float shadowTest(vec2 texcoods) {
	float shadow = texture(tex3, texcoods).r;
	float epsilon = 0.000001;
	if (shadow + epsilon < lightSpaceVertex.z) {
		return 0.5; // shadowed
	}
	return 1.0; // not shadowed
}

void main(void){

	vec2 scaledTexCoord = exTexCoord*texScaling;
	vec4 texel0 = texture(tex0, scaledTexCoord);
	vec4 texel3 = texture(tex3, exTexCoord);

	float zBuffer = texel3.x;
    zBuffer = 2.0 * zBuffer - 1.0;
	float zNear = 0.1; 
	float zFar = 1000.0;
    float zNormalized = 2.0 * zNear * zFar / (zFar + zNear - zBuffer * (zFar - zNear));
	zNormalized = zNormalized/zFar;
	
	//outColor = phongShading()*texel0;
	//outColor = vec4(vec3(zNormalized), 1);
	//outColor = vec4(vec3(lightSpaceVertex.z/zFar), 1);
	
	outColor = phongShading()*shadowTest(lightSpaceVertex.xy)*texel0;
	
	vec4 fogColor = vec4(0.8,0.8,0.8,1.0);
	outColor = mix(fogColor, outColor, fogBlending());
}





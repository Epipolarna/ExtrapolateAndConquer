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
uniform float specialValue;
	

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

uniform mat4 mMatrix;
uniform mat4 vMatrix;

uniform vec4 color;
uniform float incr;	// [0 1]

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
	float specularComponent = pow(max(dot(reflection, cameraDirection), 0.0), specularExponent);
	*/
	
	//float shading = ambientCoeff + diffuseCoeff*diffuseComponent + specularCoeff*specularComponent;
	float shading = ambientCoeff + diffuseCoeff*diffuseComponent;
	
	return shading;
}

float fogBlending()
{
	
	float depth = length((vMatrix * vec4(exPosition,1)).xyz);
	
	// WTF?
	//float depth = 1;
	//if(exPosition.y < 0)
	//	depth = -exPosition.y*22;
		
	float density = 0.005;
    const float e = 2.71828182845904523536028747135266249;
	
    float blendingFactor = pow(e, -pow(depth*density, 2.0));
	
    return blendingFactor;
}  

int nShadows = 0;
float shadowCoeff = 0.5;

float shadowTest(vec2 texcoods, float kernelSize) {
	float shadow = 0;
	float depthComparison = 0;
	float epsilon = 0.001;
	
	float texOffset = 0.7/(kernelSize*2048); // Motsvarar spridning på skuggan
	
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods).r;
	if(depthComparison > epsilon){
		shadow += 0.2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(-texOffset,-texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 0.2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(-texOffset,texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 0.2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(texOffset,-texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 0.2;
	}
	depthComparison = lightSpaceVertex.z - texture(tex3, texcoods+vec2(texOffset,texOffset)).r;
	if(depthComparison > epsilon){
		shadow += 0.2;
	}
	
	return (ambientCoeff + (1 - shadow)*(1-ambientCoeff));
}

vec4 mixTextures(vec4 texture1, vec4 texture2, float x, float start, float stop, float exp)
{
	return mix(texture1, texture2, clamp(pow((x-start)/(stop-start),exp), 0, 1));
}

vec4 blendTextures(sampler2D sandTexture, sampler2D grassTexture, sampler2D rockTexture)
{
	float height = exPosition.y;
	float PI = 3.14159265358979323846264;
	vec2 texCoord = exTexCoord;
	
	// If under water

	vec2 scaledTexCoord = texCoord*texScaling;
	/*
	if(height < 0.05 && height >= 0){
		//scaledTexCoord.y -= incr;
		//scaledTexCoord.x += sin(incr*2*PI/10);
		scaledTexCoord.y += sin(incr*2*PI)*0.01;
	}*/
	
	float horizontal = dot(exNormal, vec3(0,1,0));
				
	float grassStart = 0.1;
	float sandEnd = 0.6;
	float rockStart = 3;
	float grassEnd = 14;
	
	/*
	
	vec2 n = normalize(vec2(exNormal.x, exNormal.z));
	float rad = atan(n) + specialValue;
	
	float X = cos(rad);
	float Z = sin(rad);
	
	vec2 sandTexCoord = (mat2( X, -Z,
						       Z,  X  ) * (exTexCoord - vec2(0.5,0.5)) + vec2(0.5,0.5)) * texScaling;
							   
	
	sandTexCoord = mod(sandTexCoord, 1);
	vec4 sand = texture(sandTexture, sandTexCoord);
	*/
	
	vec4 sand = texture(sandTexture, scaledTexCoord);
	vec4 grass = texture(grassTexture, scaledTexCoord);
	vec4 rock = texture(rockTexture, scaledTexCoord);
	
	
	// naive
	/*
	if(height < grassStart)
		return sand;
	if(height <= sandEnd)
		return mixTextures(sand, grass, height, grassStart, sandEnd, 1);
	if(height <= rockStart)
		return grass;
	if(height <= grassEnd)
		return mixTextures(grass, rock, height, rockStart, grassEnd, 1);;
	return rock;
	*/
	
	// advanced
	if(height < grassStart)
		return sand;
	if(height <= sandEnd) {
		vec4 grassRockMix = mix(mixTextures(grass, rock, sandEnd, rockStart, grassEnd, 1/2),
							    mixTextures(grass, rock, sandEnd, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		return mix(mixTextures(sand, grassRockMix, height, grassStart, sandEnd, 2),
		           mixTextures(sand, grassRockMix, height, grassStart, sandEnd, 1), clamp(2*horizontal,0,1));
	}
	if(height <= rockStart) {
		vec4 lastMix = mix(mixTextures(sand, grass, sandEnd, grassStart, sandEnd, 2),
						   mixTextures(sand, grass, sandEnd, grassStart, sandEnd, 1), clamp(2*horizontal,0,1));
		vec4 nextMix_ = mix(mixTextures(grass, rock, rockStart, rockStart, grassEnd, 1/2),
						   mixTextures(grass, rock, rockStart, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		vec4 nextMix = mix(mixTextures(nextMix_, rock, rockStart, rockStart, grassEnd, 1/2),
							mixTextures(nextMix_, rock, rockStart, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		return mixTextures(lastMix, nextMix, rockStart, sandEnd, rockStart, 1);
	}
	if(height <= grassEnd) {
		vec4 lastMix = mix(mixTextures(grass, rock, height, rockStart, grassEnd, 1/2),
						   mixTextures(grass, rock, height, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		return mix(mixTextures(lastMix, rock, height, rockStart, grassEnd, 1/2),
		           mixTextures(lastMix, rock, height, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		}
	return rock;
	/*
	if(height < grassStart)
		return sand;
	if(height <= sandEnd) {
		return mix(mixTextures(sand, grass, height, grassStart, sandEnd, 2),
		           mixTextures(sand, grass, height, grassStart, sandEnd, 1), clamp(2*horizontal,0,1));
	}
	if(height < rockStart) {
		vec4 lastMix = mix(mixTextures(sand, grass, sandEnd, grassStart, sandEnd, 2),
						   mixTextures(sand, grass, sandEnd, grassStart, sandEnd, 1), clamp(2*horizontal,0,1));
		vec4 nextMix = mix(mixTextures(grass, rock, height, rockStart, grassEnd, 1/2),
						   mixTextures(grass, rock, height, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		return mixTextures(lastMix, nextMix, height, sandEnd, rockStart, 1);
	}
	if(height <= grassEnd) {
		vec4 lastMix = mix(mixTextures(grass, rock, height, rockStart, grassEnd, 1/2),
						   mixTextures(grass, rock, height, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		return mix(mixTextures(lastMix, rock, height, rockStart, grassEnd, 1/2),
		           mixTextures(lastMix, rock, height, rockStart, grassEnd, 2), clamp(1.5*horizontal, 0, 1));
		}
	return rock;*/
}



void main(void){

	float PI = 3.14159265358979323846264;
	
	vec2 scaledTexCoord = exTexCoord*texScaling;
	vec4 texel3 = texture(tex3, exTexCoord);
	
	// Texture blending
	vec4 texel0 = blendTextures(tex0, tex1, tex2);
	
	// Under water effects
	if(exPosition.y < 0) {
		texel0 = texel0*0.8 + vec4(0,0.1,0.2,1);
	}
	// Beach wave effects	
	float overLimit = 0.10;
	float y_speed = 3;
	float shift = 0.5;
	float y_limit1 = max(sin(incr*2*PI*y_speed), 0);
	float y_limit2 = max(sin(mod(incr+shift, 1)*2*PI*y_speed), 0);
	float x_shortness = 1;
	if(exPosition.y >= 0 && exPosition.y < overLimit) {
		vec4 addition = vec4(0,0,0,1);
		if(exPosition.y < y_limit1 && exPosition.y < overLimit*(sin(exPosition.x*x_shortness + incr*2*PI)+1)/2*y_limit1 ) {
			//texel0 = texel0*0.8 + (sin(incr*2*PI)+1)/2*vec4(0,0.1,0.2,1);
			addition += vec4(0,0.1,0.2,0)*(1-exPosition.y/overLimit);
		}
		if(exPosition.y < y_limit2 && exPosition.y < overLimit*(sin(exPosition.x*x_shortness + mod(incr+shift,1)*2*PI)+1)/2*y_limit2 ) {
			//texel0 = texel0*0.8 + (sin(incr*2*PI)+1)/2*vec4(0,0.1,0.2,1);
			addition += vec4(0,0.1,0.2,0)*(1-exPosition.y/overLimit);
		}
		if(length(addition) > 1)
			texel0 = texel0*0.8 + addition;
	}
	
	
	//vec4 texel0 = vec4(0.9,0.9,0.9,1);

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
		int kernelSize = 2;
		//shadowTest(lightSpaceVertex.xy, testKernelSize);
	
		//outColor = texel0*phongShading()*shadowTest(lightSpaceVertex.xy, kernelSize);
		outColor = texel0*shadowTest(lightSpaceVertex.xy, kernelSize);
	/*
		if(nShadows == 0){
			outColor = texel0*phongShading();
			//outColor = vec4(1,0,0,1);
		} else if(nShadows == pow(testKernelSize, 2.0)){
			outColor = texel0*ambientCoeff*ambientCoeff;
			//outColor = vec4(0,1,0,1);
		} else {
			outColor = texel0*phongShading()*shadowTest(lightSpaceVertex.xy, kernelSize);
			//outColor = vec4(0,0,1,1);
		}
		*/
	}
	
	vec4 fogColor = vec4(0.8,0.8,0.8,1.0);
	
	// Under water
	//if(exPosition.y < 0)	// Should be enabled when the skybox is colored aswell..
	//	fogColor = vec4(0.02,0.02,0.1,1.0);
	outColor = mix(fogColor, outColor, fogBlending());
	
	//outColor = vec4(vec3(zNormalized), 1);
}





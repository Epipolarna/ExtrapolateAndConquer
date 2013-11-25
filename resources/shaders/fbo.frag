#version 150

in vec2 exTexCoord;

uniform sampler2D tex0;

out vec4 outColor;

void main(void)
{
    vec4 texel0 = texture(tex0, exTexCoord);
	//outColor = texel0;
	
	float zBuffer = texel0.x;
    zBuffer = 2.0 * zBuffer - 1.0;
	float zNear = 0.1; 
	float zFar = 1000.0;
    float zNormalized = 2.0 * zNear * zFar / (zFar + zNear - zBuffer * (zFar - zNear));
	zNormalized = zNormalized/zFar;
	//outColor = texel0;
	outColor = vec4(vec3(zNormalized), 1);
	//outColor = vec4(vec3(zBuffer), 1);
	//outColor = vec4(vec3(gl_FragCoord.z*255),1);
    //outColor = vec4(1,1,1,1);
}

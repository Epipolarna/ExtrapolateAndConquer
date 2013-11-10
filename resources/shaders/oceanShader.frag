#version 150

in vec2 exTexCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec3 scale;

out vec4 outColor;

void main(void){

	vec2 scaledTexCoord = vec2(exTexCoord.x*scale.x, exTexCoord.y*scale.z);
	
	vec4 texel0 =  texture(tex0, scaledTexCoord);
	vec4 texel1 = texture(tex1, scaledTexCoord);
	//outColor = texture(tex0, exTexCoord);
	outColor = 0.6*texel0 + 0.4*texel1;
}

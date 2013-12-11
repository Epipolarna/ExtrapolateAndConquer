#version 150

in vec2 exTexCoord;
in vec3 exPosition;

uniform sampler2D tex;
uniform mat4 vMatrix;

out vec4 outColor;

void main(void){
	vec4 texel = texture(tex, exTexCoord);
	
	outColor = texel;
}

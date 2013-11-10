#version 150

in vec2 exTexCoord;

uniform sampler2D tex0;

out vec4 outColor;

void main(void){
	outColor = texture(tex0, exTexCoord);
}

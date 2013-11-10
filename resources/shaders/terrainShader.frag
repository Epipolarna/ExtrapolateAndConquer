#version 150

in vec2 exTexCoord;

uniform sampler2D tex0;
uniform sampler2D tex1;

uniform vec4 color;

out vec4 outColor;

void main(void){
	//outColor = texture(tex0, exTexCoord);
	outColor = color;
}

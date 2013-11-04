#version 150

in vec2 exTexCoord;

uniform sampler2D tex;

out vec4 outColor;

void main(void){
	
	outColor = texture(tex, exTexCoord);
    //outColor = vec4(1, 0, 1, 1);
}

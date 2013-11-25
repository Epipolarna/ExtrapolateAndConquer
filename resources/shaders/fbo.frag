#version 150

in vec2 exTexCoord;

uniform sampler2D tex0;

out vec4 outColor;

void main(void)
{
    outColor = texture2D(tex0, exTexCoord);
	//outColor = vec4(vec3(gl_FragCoord.z*255),1);
    //outColor = vec4(1,1,1,1);
}

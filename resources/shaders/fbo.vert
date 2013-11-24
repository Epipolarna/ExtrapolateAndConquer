#version 150

in vec3 vertex;
in vec2 texCoord;

out vec2 exTexCoord;

void main(void)
{
	exTexCoord = texCoord;
	gl_Position = vec4(vertex, 1.0);
}

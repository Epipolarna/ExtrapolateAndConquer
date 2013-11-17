#version 150

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;

out vec3 exPosition;
out vec3 exNormal;
out vec3 exTexCoord;

uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main(void){

	mat4 mpvMatrix = mMatrix * vMatrix * pMatrix;
	
	exPosition = position;
	exNormal = normal;
	exTexCoord = texCoord;

    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}

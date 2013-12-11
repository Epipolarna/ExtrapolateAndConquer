#version 150

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;

uniform mat4 vMatrix;
uniform mat4 pMatrix;
uniform mat4 mMatrix[100];

out vec3 exPosition;
out vec3 exNormal;
out vec2 exTexCoord;

void main(void){

	mat4 mpvMatrix = pMatrix * vMatrix * mMatrix[gl_InstanceID];

	exPosition = vec3(mMatrix[gl_InstanceID]*vec4(vertex,1));;
	exTexCoord = texCoord;

    gl_Position = mpvMatrix * vec4(vertex, 1.0);
}

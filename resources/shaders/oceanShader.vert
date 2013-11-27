#version 150

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;
uniform mat4 lightSourceVMatrix;

out vec3 exPosition;
out vec3 exNormal;
out vec2 exTexCoord;
out vec4 lightSpaceVertex;

void main(void)
{
	exPosition = vec3(mMatrix*vec4(vertex,1));
	exNormal = mat3(mMatrix)*normal;
	exTexCoord = texCoord;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
	
	lightSpaceVertex = pMatrix * lightSourceVMatrix * mMatrix * vec4 (vertex, 1.0);
	lightSpaceVertex.xyz /= lightSpaceVertex.w;
	lightSpaceVertex.xyz += 1.0;
	lightSpaceVertex.xyz *= 0.5;
}

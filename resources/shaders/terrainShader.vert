#version 150

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;
uniform mat4 lightSourceVMatrix;
uniform mat4 lightSourcePMatrix;

uniform mat4 lightSourcePMatrix1;
uniform mat4 lightSourcePMatrix2;
uniform mat4 lightSourcePMatrix3;

out vec3 exPosition;
out vec3 exNormal;
out vec2 exTexCoord;
out vec4 lightSpaceVertex;

out vec4 lightSpaceVertex1;
out vec4 lightSpaceVertex2;
out vec4 lightSpaceVertex3;

void main(void)
{
	exPosition = vec3(mMatrix*vec4(vertex,1));
	exNormal = mat3(mMatrix)*normal;
	exTexCoord = texCoord;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
	
	lightSpaceVertex = lightSourcePMatrix * lightSourceVMatrix * mMatrix * vec4 (vertex, 1.0);
	lightSpaceVertex.xyz /= lightSpaceVertex.w;
	lightSpaceVertex.xyz += 1.0;
	lightSpaceVertex.xyz *= 0.5;
	
	lightSpaceVertex1 = lightSourcePMatrix1 * lightSourceVMatrix * mMatrix * vec4 (vertex, 1.0);
	lightSpaceVertex1.xyz /= lightSpaceVertex1.w;
	lightSpaceVertex1.xyz += 1.0;
	lightSpaceVertex1.xyz *= 0.5;
	
	lightSpaceVertex2 = lightSourcePMatrix2 * lightSourceVMatrix * mMatrix * vec4 (vertex, 1.0);
	lightSpaceVertex2.xyz /= lightSpaceVertex2.w;
	lightSpaceVertex2.xyz += 1.0;
	lightSpaceVertex2.xyz *= 0.5;
	
	lightSpaceVertex3 = lightSourcePMatrix3 * lightSourceVMatrix * mMatrix * vec4 (vertex, 1.0);
	lightSpaceVertex3.xyz /= lightSpaceVertex3.w;
	lightSpaceVertex3.xyz += 1.0;
	lightSpaceVertex3.xyz *= 0.5;
}

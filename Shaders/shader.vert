#version 130

in vec4 vertex;
in vec3 normal;

out vec3 exNormal;
out vec3 exPosition;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main(void)
{
    gl_Position = mvpMatrix * vertex;
    exPosition = vec3(mMatrix * vertex);
    exNormal = normal;
}

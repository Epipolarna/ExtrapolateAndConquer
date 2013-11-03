#version 150

in  vec3 vertex;

uniform mat4 mvpMatrix;

void main(void)
{
    gl_Position = mvpMatrix * vec4(vertex, 1.0); // This should include projection
}

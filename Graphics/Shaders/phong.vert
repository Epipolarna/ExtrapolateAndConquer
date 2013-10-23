#version 150

in  vec3 vertex;
in  vec3 normal;
out vec3 exNormal; // Phong
out vec3 exSurface; // Phong (specular)

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

void main(void)
{
    exNormal = inverse(transpose(mat3(vMatrix * mMatrix))) * normal; // Phong, "fake" normal transformation

    exSurface = vec3(vMatrix * mMatrix * vec4(vertex, 1.0)); // Don't include projection here - we only want to go to view coordinates

    gl_Position = mvpMatrix * vec4(vertex, 1.0); // This should include projection
}

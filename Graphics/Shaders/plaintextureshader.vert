
in vec4 vertex;
in vec2 texCoord;

uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec2 outTexCoord;

void main(void)
{
        outTexCoord = texCoord;
        //gl_Position = pMatrix * vMatrix * vec4(vertex, 1.0);
        //gl_Position = pMatrix * vMatrix * vertex;
        gl_Position = vertex;
}

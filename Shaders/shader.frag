#version 130

in vec3 exPosition;
in vec3 exNormal;

uniform vec4 color;

uniform mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform mat4 vMatrix;
uniform mat4 pMatrix;

out vec4 fragColor;

void main(void)
{

    vec3 lightPosition = vec3(300, 300, -200);
    vec3 lightColor = vec3(1, 1, 1);

    float ambientCoeff = 0.5;
    float diffuseCoeff = 0.5;
    float specularCoeff = 0;


    vec3 normal = normalize(mat3(mMatrix) * exNormal);

    vec3 lightDirection = normalize(lightPosition - exPosition);
    //vec3 reflection = normalize(2 * normal * dot(lightDirection, normal) - lightDirection);
    //vec3 cameraDirection = normalize(cameraPosition - exPosition);

    float diffuseComponent = max(dot(normal, lightDirection), 0);
    //float specularComponent = pow(max(dot(reflection, cameraDirection), 0), specularExponent);

    vec3 shade = (ambientCoeff + diffuseCoeff*diffuseComponent) * lightColor;

    //fragColor = color * vec4(0.5, 0.5, 0.5, 1);

    fragColor = color * vec4(shade, 1);
}

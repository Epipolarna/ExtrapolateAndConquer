#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will follow the camera.
// You usually give light sources in world coordinates.

out vec4 outColor;
in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

void main(void)
{
    vec3 lightColor = vec3(1, 1, 1);

    float ambientCoeff  = 0.2;
    float diffuseCoeff  = 0.6;
    float specularCoeff = 100;
    float specularExponent = 50;

    const vec3 light = normalize(vec3(1, 1, 1)); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
    float diffuse, specular, shade;

    // Diffuse
    diffuse = dot(normalize(exNormal), light);
    diffuse = max(0.0, diffuse); // No negative light

    // Specular
    vec3 r = reflect(-light, normalize(exNormal));
    vec3 v = normalize(-exSurface); // View direction
    specular = dot(r, v);
    if (specular > 0.0)
            specular = 1.0 * pow(specular, specularExponent);
    specular = max(specular, 0.0);

    shade = ambientCoeff + diffuseCoeff*diffuse + specularCoeff*specular;
    outColor = vec4(shade * lightColor, 1);
    //outColor = vec4(1, 1, 1, 1);
}

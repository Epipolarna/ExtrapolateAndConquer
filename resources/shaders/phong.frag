#version 150

// Simplified Phong: No materials, only one, hard coded light source
// (in view coordinates) and no ambient

// Note: Simplified! In particular, the light source is given in view
// coordinates, which means that it will follow the camera.
// You usually give light sources in world coordinates.

in vec3 exNormal; // Phong
in vec3 exSurface; // Phong (specular)

uniform float ambientCoeff;
uniform float diffuseCoeff;
uniform float specularCoeff;
uniform float specularExponent;

uniform vec4 color;

out vec4 outColor;

void main(void)
{
    const vec3 light = normalize(vec3(1, 1, 1)); // Given in VIEW coordinates! You usually specify light sources in world coordinates.
    float diffuse, specular, shading;

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

    shading = ambientCoeff + diffuseCoeff*diffuse + specularCoeff*specular;
    outColor = vec4(color.rgb*shading, color.a);
    //outColor = vec4(1, 1, 1, 1);
}

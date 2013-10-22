#version 330
in vec2 outTexCoord;
uniform sampler2D tex;
out vec4 out_Color;

void main(void)
{
    vec4 pixel = texture(tex, outTexCoord);
    float thresh = 1;
    pixel.x = max(pixel.x - thresh, 0);
    pixel.y = max(pixel.y - thresh, 0);
    pixel.z = max(pixel.z - thresh, 0);
    //out_Color = texture(tex, outTexCoord);
    out_Color = pixel;
}

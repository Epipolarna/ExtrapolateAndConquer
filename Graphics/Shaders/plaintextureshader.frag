#version 150

in vec2 outTexCoord;
uniform sampler2D tex;
out vec4 out_Color;

void main(void)
{
    out_Color = texture2D(tex, outTexCoord);
    //out_Color = vec4(1,1,1,1);
}

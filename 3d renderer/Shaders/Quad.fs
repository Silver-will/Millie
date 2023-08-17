#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D colorTex;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform float gamma;
uniform bool bloom;
uniform bool hdr;

void main()
{             
    vec3 col = texture(colorTex, TexCoords).rgb;
    vec3 bloomCol = texture(bloomBlur, TexCoords).rgb;
    if(bloom)
    col += bloomCol;
    vec3 result = hdr ? vec3(1.0) - exp(-col * exposure) : col;
    result = pow(result, vec3(1.0/gamma));
    FragColor = vec4(result, 1.0f);
}
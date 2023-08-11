#version 450 core
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
    //col += bloom ? bloomCol : vec3(0.0f);
    col = hdr ? vec3(1.0) - exp(-col * exposure) : col;
    col = pow(col, vec3(1.0/gamma));
    FragColor = vec4(col, 1.0f);
}
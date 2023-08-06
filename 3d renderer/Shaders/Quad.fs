#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D colorTex;
uniform float exposure;
uniform bool hdr;

void main()
{             
    vec3 col = texture(colorTex, TexCoords).rgb;
    FragColor = hdr ? vec4(vec3(1.0) - exp(-col * exposure), 1.0) : vec4(col,1.0);
}
#version 450 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D colorTex;

void main()
{             
    vec3 col = texture(colorTex, TexCoords).rgb;
    FragColor = vec4(col, 1.0);
}
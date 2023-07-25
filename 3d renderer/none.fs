#version 330 core
out vec4 FragColor;
in vec2 Tex;
void main()
{
    FragColor = texture(texture_diffuse1, Tex);
}
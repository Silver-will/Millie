#version 330 core
out vec4 FragColor;

uniform samplerCube skybox;
in vec3 Tex;

void main()
{
    FragColor = texture(skybox, Tex);
}
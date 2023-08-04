#version 450 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat3 normalMatrix;
void main()
{
	gl_Position = model * vec4(aPos, 1.0f);
}
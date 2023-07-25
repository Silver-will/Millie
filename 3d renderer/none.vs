#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 aBiTan;

out vec2 Tex;
void main()
{
	Tex = aTex;

}
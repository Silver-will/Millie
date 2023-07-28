#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 Tex;
uniform mat4 model;
uniform mat4 view_r;

void main()
{
	Tex = aPos;
	vec4 pos = projection * view_r * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
}
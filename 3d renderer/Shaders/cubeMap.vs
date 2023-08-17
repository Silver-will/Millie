#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 Tex;
layout (std140, binding = 0) uniform Matrices
{
	uniform mat4 proj;
	uniform mat4 view;
};
uniform mat4 view_r;


void main()
{
	Tex = aPos;
	vec4 pos = proj * view_r * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}
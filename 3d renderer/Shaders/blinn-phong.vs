#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;

out VS_OUT
{
	vec2 Tex;
	vec3 Norm;
	vec3 FragPos;
} vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
	vs_out.FragPos = vec3 (model * vec4(pos ,1.0));
	vs_out.Norm = normalMatrix * aNorm;
	vs_out.Tex = aTex;
	gl_Position = proj * view * vec4(vs_out.FragPos, 1.0);
}
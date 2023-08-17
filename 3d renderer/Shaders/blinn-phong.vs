#version 460 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aTan;

out VS_OUT
{
	vec2 Tex;
	vec3 FragPos;
	mat3 TBN;
	mat4 viewOut;
} vs_out;

layout (std140, binding = 0) uniform Matrices
{
	uniform mat4 proj;
	uniform mat4 view;
};

uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
	vs_out.FragPos = vec3 (model * vec4(pos ,1.0));
	vec3 normal = normalMatrix * aNorm;
	vs_out.Tex = aTex;
	vec3 T = normalize(vec3(normalMatrix * aTan));
	vec3 N = normalize(normal);
	T = normalize(T - dot(T,N) * N);
	vec3 B = cross(N, T);

	vs_out.TBN = mat3(T,B,N);
	vs_out.viewOut = view;
	gl_Position = proj * view * vec4(vs_out.FragPos, 1.0);
}
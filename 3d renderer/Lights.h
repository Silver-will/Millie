#ifndef LIGHTS_H
#define LIGHTS_H
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<string>
#include<memory>
#include"Shader.h"
using namespace glm;
struct Light
{
	Light();
	vec3 ambient;
	vec3 direction;
	vec3 specular;
	void virtual UpdateVecs(Shader& s) = 0;
private:
};

struct PointLight : public Light
{
	PointLight(vec3& ambient,vec3& diffusion, vec3& position, vec3& specular, std::string name);
	vec3 position;
	vec3 ambient;
	vec3 diffusion;
	vec3 specular;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	static int pointCount;
};

struct SpotLight : public Light
{
	SpotLight(vec3& ambient,vec3& diffusion, vec3& specular, std::string& name, vec3& direction, vec3& position);
	vec3 direction;
	vec3 position;

	vec3 ambient;
	vec3 diffusion;
	vec3 specular;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	GLfloat cutOff;
	GLfloat OuterCutoff;

	static int spotCount;
};

struct DirLight : public Light
{
	DirLight(vec3& ambient, vec3& diffusion, vec3& specular, std::string& name, vec3& direction);
	vec3 direction;
	vec3 ambient;
	vec3 diffusion;
	vec3 specular;
};
#endif
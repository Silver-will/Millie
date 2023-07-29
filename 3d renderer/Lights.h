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
	vec3 ambient;
	vec3 direction;
	vec3 specular;
	void virtual UpdateVecs(Shader& s, size_t ind) = 0;
	void virtual setAttenuation(GLfloat& L, GLfloat& C, GLfloat& Q) = 0;
private:
};

struct PointLight : public Light
{
	PointLight(vec3 amb, vec3 diff, vec3 spec, vec3 pos,
		GLfloat con = 0.0f, GLfloat line = 0.0f, GLfloat quad = 0.0f);
	PointLight();
	void setAttenuation(GLfloat& L, GLfloat& C, GLfloat& Q) override;
	void UpdateVecs(Shader& s, size_t ind) override;
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;
};

struct SpotLight : public Light
{
	SpotLight(vec3 amb,vec3 diff, vec3 spec, vec3 dir, vec3 pos, GLfloat cut = 0.0f, 
		GLfloat out = 0.0f, GLfloat con = 0.0f, GLfloat line = 0.0f, GLfloat quad = 0.0f);
	SpotLight();
	void setAttenuation(GLfloat& L, GLfloat& C, GLfloat& Q) override;
	void UpdateVecs(Shader& s, size_t ind) override;
	vec3 direction;
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	GLfloat constant;
	GLfloat linear;
	GLfloat quadratic;

	GLfloat cutOff;
	GLfloat OuterCutoff;
};

struct DirLight
{
	DirLight(vec3 amb, vec3 diff, vec3 spec, vec3 dir);
	void UpdateVecs(Shader& shad);
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
namespace Light_values
{
	extern DirLight direct;
	extern std::vector<SpotLight> spots;
	extern std::vector<PointLight> points;
	extern GLfloat shine;
	extern GLfloat gamma;
}
#endif
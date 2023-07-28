#include "Lights.h"
#include<iostream>
PointLight::PointLight(vec3 amb, vec3 diff, vec3 spec, vec3 pos,
	 GLfloat con, GLfloat line, GLfloat quad)
		:ambient{ amb }, diffuse{ diff }, specular{ spec }, position{ pos }, constant{ con },
			linear{ line }, quadratic{ quad }
{
	
}
PointLight::PointLight()
{
	this->ambient = vec3(0.0f);
	this->diffuse = vec3(0.0f);
	this->specular = vec3(0.0f);
	this->position = vec3(0.0f);
	this->constant = 0.0f;
	this->linear = 0.0f;
	this->quadratic = 0.0f;
}

void PointLight::setAttenuation(GLfloat& L, GLfloat& C, GLfloat& Q)
{
	linear = L;
	constant = C;
	quadratic = Q;
}

void PointLight::UpdateVecs(Shader& s, size_t ind)
{
	string n = std::to_string(ind);
	string p = "points[";
	s.SetVector3f(p + n + "].position", position);
	s.SetVector3f(p + n + "].ambient", ambient);
	s.SetVector3f(p + n + "].diffuse", diffuse);
	s.SetVector3f(p + n + "].specular", specular);

	s.SetFloat(p + n + "].constant", constant);
	s.SetFloat(p + n + "].linear", linear);
	s.SetFloat(p + n + "].quadratic", quadratic);
	s.SetInteger("point_count", Light_values::points.size());
}

SpotLight::SpotLight(vec3 amb, vec3 diff, vec3 spec, vec3 dir, vec3 pos, GLfloat cut,
	GLfloat out, GLfloat con, GLfloat line, GLfloat quad)
	:ambient{ amb }, diffuse{ diff }, specular{ spec }, direction{ dir }, position {pos}, 
		cutOff{ cut }, OuterCutoff{ out }, quadratic{quad}, constant{con},linear{line}
{
}

SpotLight::SpotLight()
{
	this->ambient = vec3(0.0f);
	this->diffuse = vec3(0.0f);
	this->specular = vec3(0.0f);
	this->position = vec3(0.0f);
	this->direction = vec3(0.0f);
	this->constant = 0.0f;
	this->linear = 0.0f;
	this->quadratic = 0.0f;
	this->OuterCutoff = 0.0f;
	this->cutOff = 0.0f;
}

void SpotLight::setAttenuation(GLfloat& L, GLfloat& C, GLfloat& Q)
{
	linear = L;
	constant = C;
	quadratic = Q;
}

void SpotLight::UpdateVecs(Shader& s, size_t ind)
{
	string n = std::to_string(ind);
	string p = "spots[";
	s.SetVector3f(p + n + "].position", position);
	s.SetVector3f(p + n + "].direction", direction);
	s.SetVector3f(p + n + "].ambient", ambient);
	s.SetVector3f(p + n + "].diffuse", diffuse);
	s.SetVector3f(p + n + "].specular", specular);

	s.SetFloat(p + n + "].constant", constant);
	s.SetFloat(p + n + "].linear", linear);
	s.SetFloat(p + n + "].quadratic", quadratic);

	s.SetFloat(p + n + "].cutOff", cutOff);
	s.SetFloat(p + n + "].outerCutOff", OuterCutoff);
	
	s.SetInteger("spot_count", Light_values::spots.size());
}

DirLight::DirLight(vec3 amb, vec3 diff, vec3 spec, vec3 dir)
	: ambient{ amb }, diffuse{ diff }, specular{ spec }, direction{ dir }
{

}

void DirLight::UpdateVecs(Shader& s)
{
	string p = "direct";
	s.SetVector3f(p + ".direction", direction);
	s.SetVector3f(p + ".ambient", ambient);
	s.SetVector3f(p + ".diffuse", diffuse);
	s.SetVector3f(p + ".specular", specular);
}

namespace Light_values
{
	DirLight direct(vec3(0.2f), vec3(0.3f), vec3(1.0f), vec3(0.0f, -1.0f, -0.5f));
	std::vector<SpotLight> spots{};
	std::vector<PointLight> points{};
	GLfloat shine{ 32.0f};
}
#include "Lights.h"
#include<iostream>
void generatePointFBO(Shader& s)
{
		PointLight* p = &Light_values::points[0];
		glGenFramebuffers(1, &p->depthFBO);

}
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

void setLight()
{
	std::vector<glm::vec3> pointPos{
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(-2.0f, 0.9f, -1.0f),
		glm::vec3(0.0f, -10.9f, -2.0f)
	};
	for (auto& pos : pointPos)
	{
		Light_values::points.push_back(PointLight(vec3(0.15), vec3(3.0), vec3(1.0), pos, 1.0f, 0.7f, 1.8f));
	}
	std::vector<glm::vec3> spotPos
	{
		glm::vec3(0.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, -0.5f, -2.0f),
		glm::vec3(2.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, -0.5f, -2.0f)
	};

	for (size_t i = 0; i < spotPos.size(); i += 2)
	{
		Light_values::spots.push_back(SpotLight(vec3(0.0f), vec3(0.4), vec3(1.0), spotPos[i + 1], spotPos[i],
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)), 1.0f, 0.22f, 0.2f));
	}

}

namespace Light_values
{
	DirLight direct(vec3(0.2f), vec3(0.5f), vec3(1.0f), vec3(0.0f, -8.0f, 4.0f));
	std::vector<SpotLight> spots{};
	std::vector<PointLight> points{};
	GLfloat shine{ 128.0f};
	GLfloat gamma{ 2.2f };
	GLfloat exposure{1.0f};
	bool hdr{ true };
	bool bloom{ false };
}

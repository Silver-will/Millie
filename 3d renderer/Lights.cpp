#include "Lights.h"

void generatePointShadow(GLuint& depthFBO, GLuint& depthCubeMap)
{
	const GLuint HEIGHT = 1024, WIDTH = 1024;
	glGenFramebuffers(1, &depthFBO);

	glGenTextures(1, &depthCubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
	for (size_t i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
PointLight::PointLight(vec3 amb, vec3 diff, vec3 spec, vec3 pos,
	 GLfloat con, GLfloat line, GLfloat quad, bool shad)
		:ambient{ amb }, diffuse{ diff }, specular{ spec }, position{ pos }, constant{ con },
			linear{ line }, quadratic{ quad }, shadow{shad}
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
	GLfloat out, GLfloat con, GLfloat line, GLfloat quad, bool shad)
	:ambient{ amb }, diffuse{ diff }, specular{ spec }, direction{ dir }, position {pos}, 
		cutOff{ cut }, OuterCutoff{ out }, quadratic{quad}, constant{con},linear{line}, shadow{shad}
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
	this->shadow = false;
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
		glm::vec3(2.0f, 0.9f, -3.0f),
		glm::vec3(-2.0f, 0.9f, -1.0f),
		glm::vec3(0.0f, 0.9f, -2.0f)
	};
	for (auto& x : pointPos)
	{
		Light_values::points.push_back(PointLight(vec3(0.15), vec3(0.4), vec3(0.5), x, 1.0f, 0.22f, 0.2f));
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
	DirLight direct(vec3(0.2f), vec3(0.3f), vec3(1.0f), vec3(0.0f, -1.0f, -0.5f));
	std::vector<SpotLight> spots{};
	std::vector<PointLight> points{};
	GLfloat shine{ 128.0f};
	GLfloat gamma{ 2.2f };
}

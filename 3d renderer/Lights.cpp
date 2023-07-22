#include "Lights.h"
PointLight::PointLight(vec3& amb, vec3& diff, vec3& spec, vec3& pos)
	:ambient{amb}, diffuse{diff}, specular{spec}, position{pos}
{
	pointCount++;

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
}

SpotLight::SpotLight(vec3& amb, vec3& diff, vec3& spec, vec3& dir, vec3& pos, GLfloat& cut, GLfloat& out)
	:ambient{ amb }, diffuse{ diff }, specular{ spec }, position{ pos }, cutOff{cut}, OuterCutoff{out}
{
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
	s.SetVector3f(p + n + "].ambient", ambient);
	s.SetVector3f(p + n + "].diffuse", diffuse);
	s.SetVector3f(p + n + "].specular", specular);

	s.SetFloat(p + n + "].constant", constant);
	s.SetFloat(p + n + "].linear", linear);
	s.SetFloat(p + n + "].quadratic", quadratic);

	s.SetFloat(p + n + "].cutOff", cutOff);
	s.SetFloat(p + n + "].outerCutOff", OuterCutoff);
}

#pragma once
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<string>

#include"Shader.h"
using namespace glm;
struct PointLight
{
	PointLight(vec3& ambient, vec3& direction, vec3& specular, std::string name);
	vec3 ambient;
	vec3 direction;
	vec3 specular;
private:
	void UpdateVecs(Shader& s);
};


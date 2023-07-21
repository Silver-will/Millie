#ifndef AABB_H
#define AABB_H
#include<glm/glm.hpp>
#include"General_utility.h"
struct AABB 
{
	vec3 min;
	vec3 max;
	AABB(vec3 M, vec3 m): max{M},min{m} {}
	AABB(): min(vec3(0.0f)), max(vec3(0.0f)) {}
	bool box_box_intersection(AABB& b)const;
	void update(AABB &b, glm::mat3& m, glm::vec3 & t);
	
};
bool box_ray_intersection(glm::vec3 orig, const AABB& aabb, glm::vec3 ray, GLfloat& t0);
#endif // !AABB

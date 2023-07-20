#include"AABB.h"
#include<numeric>

bool AABB::box_box_intersection(AABB& b) const
{
	//test for collision
	if (this->max.x < b.min.x || this->min.x > b.max.x) return false;
	if (this->max.y < b.min.y || this->min.y > b.max.y) return false;
	if (this->max.z < b.min.z || this->min.z > b.max.z) return false;

	return true;
}

bool box_ray_intersection(glm::vec3 orig, const AABB& aabb, glm::vec3 ray, GLfloat& t0)
{
	t0 = 0.0f;
	float tMax = std::numeric_limits<float>::max();
	for (size_t i = 0; i < 3; i++)
	{
		if (abs(ray[i]) < std::numeric_limits<float>::epsilon())
		{
			if (orig[i] < aabb.min[i] || orig[i] > aabb.max[i]) return false;
		}
		else
		{
			float ood = 1.0f / ray[i];
			float t1 = (aabb.min[i] - ray[i]) * ood;
			float t2 = (aabb.max[i] - ray[i]) * ood;

			if (t1 > t2) std::swap(t1, t2);

			if (t1 > t0) t0 = t1;
			if (t2 > tMax) tMax = t2;

			if (t0 > tMax) return false;
		}
	}
	return true;
}

void AABB::update(AABB& b, glm::mat3& m, glm::vec3& t)
{
	for (size_t i = 0; i < 3; i++)
	{
		b.min[i] = b.max[i] = t[i];
		for (size_t j = 0; j < 3; j++)
		{
			float e = m[i][j] * this->min[j];
			float f = m[i][j] * this->max[j];
			if (e < f)
			{
				b.min[i] += e;
				b.max[i] += f;
			}
			else
			{
				b.min[i] += e;
				b.max[i] += f;
			}
		}
	}
}
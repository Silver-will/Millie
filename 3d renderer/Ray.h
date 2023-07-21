#ifndef RAY_H
#define RAY_H
#include<glm/glm.hpp>
#include"camera.h"
struct Ray
{
	Ray(int x_pos, int y_pos, glm::mat4& proj, glm::mat4& view, Camera& cam, GLFWwindow* window);
	Ray() : ray_orig(0.0f), ray_dir(0.0f), projection(0.0f) {}
	glm::vec3 ray_orig;
	glm::vec3 ray_dir;
	glm::mat4 projection;
};


#endif // !RAY




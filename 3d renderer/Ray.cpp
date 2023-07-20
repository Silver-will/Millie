#include "Ray.h"
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>

Ray::Ray(int x_pos, int y_pos, glm::mat4& proj, glm::mat4& view, Camera& cam, GLFWwindow* window)

{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	GLfloat x = (2.0 * x_pos) / width - 1;
	GLfloat y = 1.0 - (2.0 * y_pos) / height;
	glm::vec2 rayNs(x, y);
	glm::vec4 rayClip = glm::vec4(rayNs.x, rayNs.y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 ray_w = (glm::inverse(cam.getView()) * rayEye);
	glm::vec3 ray_wor = glm::vec3(ray_w.x, ray_w.y, ray_w.z);
	ray_dir = glm::normalize(ray_wor);
	ray_orig = cam.getPos();
}

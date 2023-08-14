#ifndef CAMERA_H
#define CAMERA_H
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
struct Camera
{
	Camera();
	void processInput(GLFWwindow* window);
	void setTarget(glm::vec3 direction);
	void updateCameraVector();
	glm::mat4 getView();
	glm::vec3 getPos();
	glm::vec3 getTarget();
	glm::vec3 getDir();
	GLfloat zoom;

private:
	glm::vec3 upValue;
	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::mat4 view;
	glm::vec3 Right;
};
namespace Camera_values {
	extern GLfloat deltaTime;
	extern GLfloat lastFrame;
	extern GLfloat currentFrame;
	extern Camera cam;
}

#endif
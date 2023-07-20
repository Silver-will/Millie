#include<glm/gtc/type_ptr.hpp>
#include "camera.h"
using namespace glm;
namespace Camera_values {
	GLfloat deltaTime{};
	GLfloat lastFrame{};
	GLfloat currentFrame{};

}
Camera::Camera()
{
	upValue = vec3(0.0f, 1.0f, 0.0f);
	cameraPos = vec3(0.0f,0.0f,3.0f);
	cameraTarget =vec3(0.0f, 0.0f, -1.0f);
	Right = normalize(cross(cameraTarget, upValue));
	view = glm::lookAt(cameraPos, cameraTarget, upValue);
}

void Camera::processInput(GLFWwindow* window)
{
	Camera_values::currentFrame = glfwGetTime();
	Camera_values::deltaTime = Camera_values::currentFrame - Camera_values::lastFrame;
	Camera_values::lastFrame = Camera_values::currentFrame;
	GLfloat speed = 2.5f * Camera_values::deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT))
	{
		cameraPos -= speed * Right;
	}
	if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT))
	{
		cameraPos += speed * Right;
	}
	if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP))
	{
		cameraPos += cameraTarget * speed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN))
	{
		cameraPos -= cameraTarget * speed;
	}
}

void Camera::updateCameraVector()
{
	
}

glm::mat4 Camera::getView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraTarget, upValue);
	return view;
}

glm::vec3 Camera::getPos()
{
	return this->cameraPos;
}

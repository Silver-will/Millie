#include"imgui/imgui.h"
#include"imgui/imgui_impl_glfw.h"
#include"imgui/imgui_impl_opengl3.h"
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>

#include"General_utility.h"
#include"camera.h"
#include"model.h"
#include"Ray.h"

//handles windows resize
void FramebufferCallback(GLFWwindow* window, GLint width, GLint height);
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);

//Ray casting algorithm projecting a ray from the mouse in world co-ordinates
void ScreenSpaceToWorldSpace(GLdouble xpos, GLdouble ypos, GLint width, GLint height);

//calculate change in mouse position and store in global namespace
void ProcessOffset(GLfloat xpos, GLfloat ypos);

void Input(GLFWwindow* window);

const int WIDTH = 1024;
const int HEIGHT = 720;
Camera cam;
glm::mat4 projection = glm::mat4(1.0f);
Ray rayWor;

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Millie", NULL, NULL);
	if (window == nullptr)
	{
		Log("Failed to load Window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glfwSetFramebufferSizeCallback(window, FramebufferCallback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);
	stbi_set_flip_vertically_on_load(true);

	Shader modelShader("./Shaders/Model.vs", "./Shaders/Model.fs");
	Model oModel("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\backpack\\backpack.obj");

	while (!glfwWindowShouldClose(window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();
		Input(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		modelShader.use();
		projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
		modelShader.SetMatrix4("proj", projection);
		glm::mat4 view(1.0f);
		view = cam.getView();
		modelShader.SetMatrix4("view", view);
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		GLfloat theta = static_cast<GLfloat>(glfwGetTime());
		model = glm::rotate(model, theta, glm::vec3(cos(theta), sin(theta), 0.0f));
		modelShader.SetMatrix4("model", model);
		oModel.Draw(modelShader);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}

void FramebufferCallback(GLFWwindow* window, GLint width, GLint height)
{
	glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos)
{
	GLint width, height;
	glfwGetWindowSize(window, &width, &height);
	ScreenSpaceToWorldSpace(xpos, ypos, width, height);
	float xPos = static_cast<float>(xpos);
	float yPos = static_cast<float>(ypos);
	ProcessOffset(xPos, yPos);
}

void ScreenSpaceToWorldSpace(GLdouble xpos, GLdouble ypos, GLint width, GLint height)
{

	GLfloat x = (2.0 * xpos) / width - 1;
	GLfloat y = 1.0 - (2.0 * ypos) / height;
	glm::vec2 rayNs(x, y);
	glm::vec4 rayClip = glm::vec4(rayNs.x, rayNs.y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(projection) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);
	glm::vec4 ray_w = (glm::inverse(cam.getView()) * rayEye);
	rayWor.ray_dir = glm::vec3(ray_w.x, ray_w.y, ray_w.z);
	rayWor.ray_dir = glm::normalize(rayWor.ray_dir);
}

void ProcessOffset(GLfloat xpos, GLfloat ypos)
{
	if (Glob::fMouse)
	{
		Glob::fMouse = false;
		Glob::lastX = xpos;
		Glob::lastY = ypos;
	}
	else
	{
		Glob::xOffset = xpos - Glob::lastX;
		Glob::yOffset = ypos - Glob::lastY;
		Glob::lastX = xpos;
		Glob::lastY = ypos;
	}
}

void Input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	cam.processInput(window);
}
#include"UI.h"
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>

#include"General_utility.h"
#include"camera.h"
#include"model.h"
#include"Ray.h"
#include"Lights.h"
#include"VAO.h"

//handles windows resize
void FramebufferCallback(GLFWwindow* window, GLint width, GLint height);
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
void blank(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
void setLight();

//Ray casting algorithm projecting a ray from the mouse in world co-ordinates
void ScreenSpaceToWorldSpace(GLdouble xpos, GLdouble ypos, GLint width, GLint height);

//calculate change in mouse position and store in global namespace
void ProcessOffset(GLfloat xpos, GLfloat ypos);
void Input(GLFWwindow* window);
void UpdatePhong(Shader& shad);

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
	bool showWindow = true;

	

	glfwSetFramebufferSizeCallback(window, FramebufferCallback);
		
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	stbi_set_flip_vertically_on_load(true);

	Shader modelShader("./Shaders/blinn-phong.vs", "./Shaders/blinn-phong.fs");
	Model oModel("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\cyborg\\cyborg.obj");
	std::vector<GLfloat> vertices = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
	};

	std::vector<GLint> offset{ 3,3,2 };
	/*VAO vao;
	vao.GenerateBuffer("VERTEX", vertices, offset);
	vao.unbind();
	*/

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -1.0f, -2.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	//GLuint diffuse = TextureFromFile("container2.png", "C:\\Users\\katsura\\Documents\\PROGRAMMING\\LearnOpenGL\\resources\\textures");
	//GLuint specular = TextureFromFile("container2_specular.png", "C:\\Users\\katsura\\Documents\\PROGRAMMING\\LearnOpenGL\\resources\\textures");
	glm::mat3 Normalmat = glm::transpose(glm::inverse(model));

	/*modelShader.use();
	modelShader.SetInteger("material.texture_diffuse1", 0);
	modelShader.SetInteger("material.texture_specular1", 1);
	*/

	PointLight P(vec3(0.15), vec3(0.4), vec3(0.5), vec3(0.5f, 0.5f, 0.5f));
	Light_values::points.push_back(P);
	Light_values::spots.push_back(SpotLight());
	GLint width{}, height{};
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		Input(window);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwGetWindowSize(window, &width, &height);
		modelShader.use();
		projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
		modelShader.SetMatrix4("proj", projection);
		modelShader.SetVector3f("viewPos", cam.getPos());
		glm::mat4 view(1.0f);
		view = cam.getView();
		modelShader.SetMatrix4("model", model);
		modelShader.SetMatrix4("view", view);
		modelShader.SetMatrix3("normalMatrix", Normalmat);

		oModel.Draw(modelShader);
		/*vao.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/
		
		UpdatePhong(modelShader);
		SetupUI(&showWindow);

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
	static GLfloat yaw = -90.0f;
	static GLfloat pitch = 0.0f;
	static GLfloat sensitivity = 0.1f;
	if (Glob::fMouse)
	{
		Glob::fMouse = false;
		Glob::lastX = xpos;
		Glob::lastY = ypos;
	}
	else
	{
		Glob::xOffset = xpos - Glob::lastX;
		Glob::yOffset = Glob::lastY - ypos;
		Glob::lastX = xpos;
		Glob::lastY = ypos;
	}
	
	Glob::xOffset *= sensitivity;
	Glob::yOffset *= sensitivity;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	yaw += Glob::xOffset;
	pitch += Glob::yOffset;

	static glm::vec3 direction;

	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cam.setTarget(glm::normalize(direction));
}

void Input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		glfwSetCursorPosCallback(window, MouseCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		glfwSetCursorPosCallback(window, NULL);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	cam.processInput(window);
}

void UpdatePhong(Shader& shad)
{
	for (size_t i = 0; i < Light_values::points.size(); i++)
	{
		Light_values::points[i].UpdateVecs(shad, i);
	}
	for (size_t i = 0; i < Light_values::spots.size(); i++)
	{
		Light_values::points[i].UpdateVecs(shad, i);
	}
	Light_values::direct.UpdateVecs(shad);
	shad.SetFloat("material.shininess", Light_values::shine);
	/*
	shad.SetInteger("point_count", Light_values::points.size());
	shad.SetInteger("spot_count", Light_values::spots.size());
	*/
}

void setLight()
{

}
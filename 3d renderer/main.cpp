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

GLuint loadCubeMapTexture(string directory);
GLuint generateUBO();
void setUboValue(glm::mat4& matrice, GLuint& ubo, GLint off);
void FramebufferCallback(GLFWwindow* window, GLint width, GLint height);
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
	//stbi_set_flip_vertically_on_load(true);

	Shader modelShader("./Shaders/blinn-phong.vs", "./Shaders/blinn-phong.fs");
	Shader skyboxShader("./Shaders/cubeMap.vs", "./Shaders/cubeMap.fs");
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

	std::vector<GLfloat> skyBox{
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};
	std::vector<GLint> offset{ 3,3,2 };
	/*VAO vao;
	vao.GenerateBuffer("VERTEX", vertices, offset);
	vao.unbind();
	*/

	std::vector<GLint> skyOffset{ 3 };
	VAO skyvao;
	skyvao.GenerateBuffer("VERTEX", skyBox, skyOffset);
	skyvao.unbind();

	GLuint skyboxMap = loadCubeMapTexture("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\skybox\\");

	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -0.5f, -2.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	//GLuint diffuse = TextureFromFile("container2.png", "C:\\Users\\katsura\\Documents\\PROGRAMMING\\LearnOpenGL\\resources\\textures");
	//GLuint specular = TextureFromFile("container2_specular.png", "C:\\Users\\katsura\\Documents\\PROGRAMMING\\LearnOpenGL\\resources\\textures");
	glm::mat3 Normalmat = glm::transpose(glm::inverse(model));

	/*modelShader.use();
	modelShader.SetInteger("material.texture_diffuse1", 0);
	modelShader.SetInteger("material.texture_specular1", 1);
	*/
	skyboxShader.use();
	skyboxShader.SetInteger("skybox", 0);
	setLight();
	GLint width{}, height{};
	GLuint ubo = generateUBO();
	glm::mat4 view(1.0f);
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
		setUboValue(projection, ubo, 0);
		modelShader.SetVector3f("viewPos", cam.getPos());
		
		view = cam.getView();
		setUboValue(view, ubo, 1);
		modelShader.SetMatrix4("model", model);
		modelShader.SetMatrix3("normalMatrix", Normalmat);
		oModel.Draw(modelShader);
		UpdatePhong(modelShader);

		glDepthFunc(GL_LEQUAL);
		skyboxShader.use();
		view = glm::mat4(glm::mat3(cam.getView()));
		skyboxShader.SetMatrix4("view_r", view);
		skyvao.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMap);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);
		/*vao.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular);

		glDrawArrays(GL_TRIANGLES, 0, 36);
		*/
		
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
		Light_values::spots[i].UpdateVecs(shad, i);
	}
	Light_values::direct.UpdateVecs(shad);
	shad.SetFloat("material.shininess", Light_values::shine);
	shad.SetFloat("gamma", Light_values::gamma);
}

void setLight()
{
	std::vector<glm::vec3> pointPos{
		glm::vec3(2.0f, 0.9f, -3.0f),
		glm::vec3(-2.0f, 0.9f, -1.0f),
		glm::vec3(0.0f, 0.9f, -2.0f)
	};
	for (auto& x : pointPos)
	{
		Light_values::points.push_back(PointLight(vec3(0.15), vec3(0.4), vec3(0.5), x, 1.0f, 0.22f, 0.2f));
	}
	std::vector<glm::vec3> spotPos
	{
		glm::vec3(0.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, -0.5f, -2.0f),
		glm::vec3(2.0f, 0.0f, -3.0f),
		glm::vec3(0.0f, -0.5f, -2.0f)
	};

	for (size_t i = 0; i < spotPos.size(); i+=2)
	{
		Light_values::spots.push_back(SpotLight(vec3(0.0f), vec3(0.4), vec3(1.0), spotPos[i + 1], spotPos[i],
			glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.0f)), 1.0f, 0.22f, 0.2f));
	}

}

GLuint loadCubeMapTexture(string directory)
{
	std::vector<string> faces{
		"right.jpg",
		"left.jpg",
		"top.jpg",
		"bottom.jpg",
		"front.jpg",
		"back.jpg",
	};
	GLuint ID;
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

	GLint width, height, nrComponents;
	
	for (size_t i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load((directory + faces[i]).c_str(), &width, &height, &nrComponents, 0);
		GLenum imageFormat{};
		if (data)
		{
			if (nrComponents == 1) imageFormat = GL_RED;
			else if (nrComponents == 3) imageFormat = GL_RGB;
			else imageFormat = GL_RGBA;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			Log("Failed to load cube map in directory: " + directory+faces[i]);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return ID;
}

GLuint generateUBO() {
	GLuint uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	return uboMatrices;
}

void setUboValue(glm::mat4& matrice, GLuint& ubo, GLint off)
{
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, off * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(matrice));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>

#include"UI.h"
#include"General_utility.h"
#include"camera.h"
#include"model.h"
#include"Lights.h"
#include"VAO.h"
#include"Shadow.h"
#include"Buffers.h"

GLuint loadCubeMapTexture(string directory);
void drawPointShadow(PointLight& light, Shader& s, GLuint& tex);
void drawLightPos();
void drawQuad();
void drawPlane(GLuint& Tex, Shader& s);
void drawCubes(GLuint& Tex, Shader& s);

void FramebufferCallback(GLFWwindow* window, GLint width, GLint height);
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
void ProcessOffset(GLfloat xpos, GLfloat ypos);
void Input(GLFWwindow* window);
void UpdatePhong(Shader& shad);
void UpdateHDR(Shader& shad);

const int WIDTH = 1440;
const int HEIGHT = 800;
Camera cam;
glm::mat4 projection = glm::mat4(1.0f);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
	Shader sceneShader("./Shaders/b_phong_no_spec.vs", "./Shaders/b_phong_no_spec.fs");
	Shader skyboxShader("./Shaders/cubeMap.vs", "./Shaders/cubeMap.fs");
	Shader DepthMapShader("./Shaders/cubeDepth.vs", "./Shaders/cubeDepth.fs", "./Shaders/cubeDepth.gs");
	Shader baseShader("./Shaders/none.vs", "./Shaders/none.fs");
	Shader quadShader("./Shaders/Quad.vs", "./Shaders/Quad.fs");
	Model Cyborg("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\cyborg\\cyborg.obj");
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
	std::vector<GLint> skyOffset{ 3 };
	VAO skyvao;
	skyvao.GenerateBuffer("VERTEX", skyBox, skyOffset);
	skyvao.unbind();

	GLuint skyboxMap = loadCubeMapTexture("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\skybox\\");
	GLuint wood = TextureFromFile("wood.png", "C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources", "texture_diffuse");
	
	FrameBuffer screenBuff;
	screenBuff.setDimensions(WIDTH, HEIGHT);
	screenBuff.attachColorTex();


	modelShader.use();
	modelShader.SetInteger("material.texture_diffuse1", 0);
	modelShader.SetInteger("depth", 1);
	skyboxShader.use();
	skyboxShader.SetInteger("skybox", 0);
	quadShader.use();
	quadShader.SetInteger("colorTex", 0);
	setLight();
	generatePointFBO(modelShader);
	
	
	GLuint ubo = generateUBO();
	glm::mat4 view(1.0f);
	GLint width{}, height{};
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glfwGetWindowSize(window, &width, &height);
		screenBuff.resizeTexture(width, height);
		screenBuff.bind();
		glViewport(0, 0, WIDTH, HEIGHT);
		
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		
		Input(window);

		/*for (auto& light : Light_values::points)
		{
			drawPointShadow(light, DepthMapShader, wood);
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		*/
		
		modelShader.use();
		UpdatePhong(modelShader);
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WIDTH/ (GLfloat)HEIGHT, 0.1f, 100.0f);
		setUboValue(projection, ubo, 0);
		view = cam.getView();
		setUboValue(view, ubo, 1);
		modelShader.SetVector3f("viewPos", cam.getPos());
		modelShader.SetVector3f("material.specular", glm::vec3(0.5f));
		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -1.5, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f));
		glm::mat3 Normalmat = glm::transpose(glm::inverse(model));
		modelShader.SetMatrix4("model", model);
		modelShader.SetMatrix3("normalMatrix", Normalmat);
		Cyborg.Draw(modelShader);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, wood);
		//glActiveTexture(GL_TEXTURE1);
		//glBindTexture(GL_TEXTURE_CUBE_MAP, Light_values::points[0].cubeMap);
		/*
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Light_values::points[1].cubeMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, Light_values::points[2].cubeMap);
		*/
		//drawCubes(wood, modelShader);
		//drawPlane(wood, modelShader);

		for (auto& x : Light_values::points)
		{
			baseShader.use();
			glm::mat4 lightModel(1.0f);
			lightModel = glm::translate(lightModel, x.position);
			lightModel = glm::scale(lightModel, glm::vec3(0.2f));
			baseShader.SetMatrix4("model", lightModel);
			baseShader.SetVector3f("color", x.diffuse);
			std::cout << x.diffuse[0] << std::endl;
			drawLightPos();
		}

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
		
		screenBuff.unbind();
		glViewport(0, 0, WIDTH, HEIGHT);

		glDisable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT);
		quadShader.use();
		screenBuff.bindTex();
		UpdateHDR(quadShader);
		drawQuad();
		
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
	float xPos = static_cast<float>(xpos);
	float yPos = static_cast<float>(ypos);
	ProcessOffset(xPos, yPos);
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
	shad.use();
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

void UpdateHDR(Shader& shad)
{
	shad.SetInteger("hdr", Light_values::hdr);
	shad.SetFloat("exposure", Light_values::exposure);
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

void drawPlane(GLuint& Tex, Shader& s)
{
	static VAO planevao;
	static bool first{ true };
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0, -1.5, 0.0f));
	glm::mat3 Normalmat = glm::transpose(glm::inverse(model));
	s.SetMatrix4("model", model);
	s.SetMatrix3("normalMatrix", Normalmat);
	if (first)
	{
		std::vector<GLfloat> planeVertices = {
			-25.0f, 0.0f, 25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
			-25.0f, 0.0f, -25.0f, 0.0f, 1.0f, 0.0f,  0.0f, 25.0f,
			-25.0f, 0.0f, 25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
			25.0f, 0.0f, 25.0f,   0.0f, 1.0f, 0.0f,  25.0f, 0.0f,
			25.0f, 0.0f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f,
		};
		std::vector<GLint> planeOffset{ 3,3,2 };
		planevao.GenerateBuffer("VERTEX", planeVertices, planeOffset);
		first = false;	
		std::cout << "cruise" << std::endl;
	}
	planevao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	planevao.unbind();
}

void drawCubes(GLuint& Tex, Shader& s)
{
	static VAO cubevao;
	static bool first{ true };
	glm::mat4 model(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, -2.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	glm::mat3 Normalmat = glm::transpose(glm::inverse(model));
	s.SetMatrix4("model", model);
	s.SetMatrix3("normalMatrix", Normalmat);
	if (first)
	{
		std::vector<GLfloat> cubeVertices{

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
		std::vector<GLint> cubeOffset{ 3,3,2 };
		cubevao.GenerateBuffer("VERTEX", cubeVertices, cubeOffset);
		first = false;
	}
	cubevao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 36);
	cubevao.unbind();
}

void drawPointShadow(PointLight& light,Shader& s, GLuint& tex)
{
	static const GLfloat nearPlane = 1.0f;
	static const GLfloat farPlane = 25.0f;
	static std::vector<glm::mat4> transforms;
	static const GLfloat WIDTH = 1024, HEIGHT = 1024;
	if (light.lastPosition != light.position)
	{
		transforms.clear();
		static glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), WIDTH/ HEIGHT, nearPlane, farPlane);
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		transforms.push_back(shadowProj * glm::lookAt(light.position, light.position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		light.lastPosition = light.position;
	}
	glViewport(0, 0, WIDTH, HEIGHT);
	light.bindFramebuffer();
	glClear(GL_DEPTH_BUFFER_BIT);
	s.use();
	for (size_t i = 0; i < 6; i++)
		s.SetMatrix4("shadowMatrices[" + std::to_string(i) + "]", transforms[i]);
	s.SetFloat("far_plane", farPlane);
	s.SetVector3f("lightPos", light.position);
	//drawCubes(tex, s);
	drawPlane(tex, s);
	light.unbindFramebuffer();
}

void drawLightPos()
{
	static bool first = true;
	static VAO lightvao;
	if (first)
	{
		std::vector<GLfloat> lightVertices = {
		-0.5f, -0.5f, -0.5f,  
		 0.5f, -0.5f, -0.5f, 
		 0.5f,  0.5f, -0.5f,  
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f, 

		-0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f, 
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f, 
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f, -0.5f, 
		 0.5f, -0.5f,  0.5f, 
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f, 
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f, 
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,  
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,  
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f, 
		-0.5f,  0.5f, -0.5f,
		};
		std::vector<GLint> lightOffset{3};
		lightvao.GenerateBuffer("VERTEX", lightVertices, lightOffset);
		first = false;
	}
	lightvao.bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 36);
}

void drawQuad()
{
	static VAO debugvao;
	static bool first = true;
	if (first)
	{
		std::vector<GLfloat> quadVertices{
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f, 
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,

			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
			
		};
		std::vector<GLint> quadOffset{ 3,2 };
		debugvao.GenerateBuffer("VERTEX", quadVertices, quadOffset);
		first = false;
	}
	debugvao.bind();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	debugvao.unbind();
}


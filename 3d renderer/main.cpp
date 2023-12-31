#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<iostream>
#include<chrono>

#include"UI.h"
#include"General_utility.h"
#include"camera.h"
#include"model.h"
#include"Lights.h"
#include"VAO.h"
#include"Shadow.h"
#include"Buffers.h"
using Camera_values::cam;
bool debug = false;
bool rotate_camera = false;
using namespace Glob;
GLuint loadCubeMapTexture(string directory);
void drawLightPos();
void drawQuad();
void drawPlane(GLuint& Tex, Shader& s);
void drawCubes(GLuint& Tex, Shader& s);

void FramebufferCallback(GLFWwindow* window, GLint width, GLint height);
void MouseCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void ProcessOffset(GLfloat xpos, GLfloat ypos);
void HandleCameraRotation(GLFWwindow* window);
void Input(GLFWwindow* window);
void UpdatePhong(Shader& shad);
void UpdateHDR(Shader& shad);
void UpdateShadowValues(Shader& shad);

const int WIDTH = 1440;
const int HEIGHT = 800;
glm::mat4 projection = glm::mat4(1.0f);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
	//glfwSetMouseButtonCallback(window, MouseButtonCallback);
		
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	
	//stbi_set_flip_vertically_on_load(true);

	Shader cascadeShader("./Shaders/cascade_depth.vs", "./Shaders/cascade_depth.fs", "./Shaders/cascade_depth.gs");
	Shader modelShader("./Shaders/blinn-phong.vs", "./Shaders/blinn-phong.fs");
	//Shader sceneShader("./Shaders/b_phong_no_spec.vs", "./Shaders/b_phong_no_spec.fs");
	Shader skyboxShader("./Shaders/cubeMap.vs", "./Shaders/cubeMap.fs");
	Shader lightShader("./Shaders/none.vs", "./Shaders/light_bloom.fs");
	Shader quadShader("./Shaders/Quad.vs", "./Shaders/Quad.fs");
	Shader blurShader("./Shaders/bloom_blur.vs", "./Shaders/bloom_blur.fs");
	Shader debugShader("./Shaders/debugMap.vs", "./Shaders/debugMap.fs");
	Model Room("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\Japanese room\\Room.obj");
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
	
	//load in textures
	GLuint skyboxMap = loadCubeMapTexture("C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources\\skybox\\");
	GLuint wood = TextureFromFile("wood.png", "C:\\Users\\katsura\\source\\repos\\3d renderer\\3d renderer\\resources", "texture_diffuse");
	
	//Hdr FrameBuffer
	FrameBuffer screenBuff(2);
	screenBuff.attachColorTex();
	screenBuff.setMRTs();
	screenBuff.checkComplete();
	screenBuff.unbind();

	generateLightFBOAndTex(lightFBO, lightFBOTex);

	unsigned int pingpongFBO[2];
	unsigned int pingpongColorbuffers[2];
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	skyboxShader.use();
	skyboxShader.SetInteger("skybox", 0);
	
	blurShader.use();
	blurShader.SetInteger("image", 0);

	quadShader.use();
	quadShader.SetInteger("colorTex", 0);
	quadShader.SetInteger("bloomBlur", 1);
	setLight();
	
	GLuint ubo = generateUBO(2 * sizeof(glm::mat4), 0);
	GLuint lightUbo = generateUBO(8 * sizeof(glm::mat4), 1);
	glm::mat4 view(1.0f);
	GLint width{}, height{};
	while (!glfwWindowShouldClose(window))
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto lightMatrices = getLightSpaceMatrices();
		for (size_t i = 0; i < lightMatrices.size(); i++)
		{
			setUboValue(lightMatrices[i], lightUbo, i);
		}
		
		glfwGetWindowSize(window, &width, &height);

		glm::mat4 model = glm::mat4(1.0f);
		

		//Render scene from light sources perspective to depth map
		cascadeShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glViewport(0, 0, 4096, 4096);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f,1.5));
		cascadeShader.SetMatrix4("model", model);
		Room.Draw(cascadeShader);

		glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//First renderpass to offscreen framebuffer

		screenBuff.bind();
		glViewport(0, 0, WIDTH, HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		Input(window);
		modelShader.use();
		UpdatePhong(modelShader);
		projection = glm::perspective(glm::radians(cam.zoom), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		setUboValue(projection, ubo, 0);
		view = cam.getView();
		setUboValue(view, ubo, 1);
		UpdateShadowValues(modelShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0, -3.0, 1.5));
		modelShader.SetVector3f("viewPos", cam.getPos());
		glm::mat3 Normalmat = glm::transpose(glm::inverse(model));
		modelShader.SetMatrix4("model", model);
		modelShader.SetMatrix3("normalMatrix", Normalmat);

		Room.Draw(modelShader);
		for (auto& x : Light_values::points)
		{
			lightShader.use();
			glm::mat4 lightModel(1.0f);
			lightModel = glm::translate(lightModel, x.position);
			lightModel = glm::scale(lightModel, glm::vec3(0.1f));
			lightShader.SetMatrix4("model", lightModel);
			lightShader.SetVector3f("lightColor", x.diffuse);
			drawLightPos();
		}

		//change depth test to less or equal to avoid skybox fragments being disgarded
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

		//second render pass to apply a gaussian blur

		bool horizontal = true, first_iteration = true;
		GLuint amount = 10;
		blurShader.use();

		//bind 2nd ping-pong buffer
		if (Light_values::bloom)
		{
			for (size_t i = 0; i < amount; i++)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
				blurShader.SetInteger("horizontal", horizontal);
				if (first_iteration)
				{
					screenBuff.bindTex(1);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
				}
				drawQuad();
				horizontal = !horizontal;
				if (first_iteration)
					first_iteration = false;
			}
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		quadShader.use();
		glActiveTexture(GL_TEXTURE0);
		screenBuff.bindTex(0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
		UpdateHDR(quadShader);
		drawQuad();
		if (debug)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			debugShader.use();
			debugShader.SetInteger("layer", 0);
			debugShader.SetFloat("near_plane", 0.1f);
			debugShader.SetFloat("far_plane", 500.0f);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D_ARRAY, lightFBOTex);
			drawQuad();
			
		}
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

void HandleCameraRotation(GLFWwindow* window)
{
	if (rotate_camera == true)
	{
		glfwSetCursorPosCallback(window, MouseCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetCursorPosCallback(window, ImGui_ImplGlfw_CursorPosCallback);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	static auto start{ std::chrono::steady_clock::now() };
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		const auto end{ std::chrono::steady_clock::now() };
		const std::chrono::duration<double> time_since_last_click{ end - start };
		Glob::fMouse = true;
		if (time_since_last_click > 0.15s)
		{
			std::cout << "r\n";
			rotate_camera = !rotate_camera;
			start = std::chrono::steady_clock::now();
		}
	}
}

void Input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		debug = true;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		debug = false;
	}
	HandleCameraRotation(window);
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
}

void UpdateHDR(Shader& shad)
{
	shad.SetInteger("hdr", Light_values::hdr);
	shad.SetInteger("bloom", Light_values::bloom);
	shad.SetFloat("exposure", Light_values::exposure);
	shad.SetFloat("gamma", Light_values::gamma);
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
	glDrawArrays(GL_TRIANGLES, 0, 36);
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

void UpdateShadowValues(Shader& shad)
{
	shad.SetFloat("far_plane", Camera_values::cameraFarPlane);
	auto& cascades = Shadow_values::shadowCascadeLevels;
	shad.SetInteger("cascadeCount", cascades.size());
	shad.SetInteger("shadow", Shadow_values::shadow);
	for (size_t i = 0; i < cascades.size(); i++)
	{
		shad.SetFloat("cascadePlaneDistances[" + std::to_string(i) + "]", cascades[i]);
	}
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
	{
		std::cout << "r\n";
		rotate_camera = !rotate_camera;
	}
}

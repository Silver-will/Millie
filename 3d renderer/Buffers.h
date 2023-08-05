#ifndef BUFFERS_
#define BUFFERS_
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
GLuint generateUBO();
void setUboValue(glm::mat4& matrice, GLuint& ubo, GLint off);

struct FrameBuffer
{
	FrameBuffer();
	~FrameBuffer();
	void attachColorTex();
	void bind();
	void unbind();
	void bindTex();
	void setDimensions(GLuint width, GLuint height);
private:
	GLuint FBO;
	GLuint RBO;
	GLuint ColorAttachment;
	GLuint depthAttachment;
	GLuint stencilAttachment;
	GLuint textureWidth;
	GLuint textureHeight;
};
#endif

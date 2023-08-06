#ifndef BUFFERS_
#define BUFFERS_
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<vector>
GLuint generateUBO();
void setUboValue(glm::mat4& matrice, GLuint& ubo, GLint off);

struct FrameBuffer
{
	FrameBuffer(GLint ColorAttachNo = 0);
	~FrameBuffer();
	void attachColorTex();
	void bind();
	void unbind();
	void bindTex();
	void setDimensions(GLuint width, GLuint height);
	void resizeTexture(GLuint width = 0.0f, GLuint height = 0.0f);
	void setMRTs();
private:
	GLuint FBO;
	GLuint RBO;
	GLuint ColorAttachment;
	GLuint depthAttachment;
	GLuint stencilAttachment;
	GLuint textureWidth;
	GLuint textureHeight;
	GLuint ColorTexNo;
	std::vector<GLuint>attachments;
};
#endif

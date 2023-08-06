#include "Buffers.h"
#include "General_utility.h"
FrameBuffer::FrameBuffer(GLint ColorAttachNo):textureWidth{1440}, textureHeight{800}
{
	this->ColorTexNo = ColorAttachNo;
	this->attachments.resize(ColorAttachNo);
	//Framebuffer set up
	glGenFramebuffers(1, &this->FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);

	//render buffer set up
	glGenRenderbuffers(1, &this->RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1440, 800);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, this->RBO);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &this->FBO);
}

void FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
}

void FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindTex()
{
	glBindTexture(GL_TEXTURE_2D, this->ColorAttachment);
}

void FrameBuffer::setDimensions(GLuint width, GLuint height)
{
	this->textureWidth = width;
	this->textureHeight = height;
}

void FrameBuffer::resizeTexture(GLuint width, GLuint height)
{
	setDimensions(width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->textureWidth, this->textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}
void FrameBuffer::attachColorTex()
{
	glGenTextures(ColorTexNo, this->attachments.data());
	for (size_t i = 0; i < ColorTexNo; i++)
	{
		glBindTexture(GL_TEXTURE_2D, this->attachments[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->textureWidth, this->textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->attachments[i], 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			Log("FRAMEBUFFER IS NOT COMPLETE");
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
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

void FrameBuffer::setMRTs()
{
	std::vector<GLuint> targets(ColorTexNo);
	for (size_t i = 0; i < targets.size(); i++)
	{
		targets[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	glDrawBuffers(ColorTexNo, targets.data());
}

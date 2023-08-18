#include "Shadow.h"
#include "camera.h"
#include<iostream>
using Camera_values::cam;
using Shadow_values::shadowCascadeLevels;
std::vector<glm::vec4> getFrustrumCornersWorldSpace(const glm::mat4& projView)
{
	const auto inv = glm::inverse(projView);

	std::vector<glm::vec4> frustumCorners;
	for (size_t x = 0; x < 2; x++)
	{
		for (size_t y = 0; y < 2; y++)
		{
			for (size_t z = 0; z < 2; z++)
			{
				const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}
	return frustumCorners;
}

std::vector<glm::mat4> getLightSpaceMatrices()
{
	std::vector<glm::mat4> ret;
	for (size_t i = 0; i < shadowCascadeLevels.size() + 1; ++i)
	{
		if (i == 0)
		{
			ret.push_back(getLightSpaceMatrix(Camera_values::cameraNearPlane, shadowCascadeLevels[i]));
		}
		else if (i < shadowCascadeLevels.size())
		{
			ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], shadowCascadeLevels[i]));
		}
		else
		{
			ret.push_back(getLightSpaceMatrix(shadowCascadeLevels[i - 1], Camera_values::cameraFarPlane));
		}
	}
	return ret;
}

glm::mat4 getLightSpaceMatrix(const GLfloat nearPlane, const GLfloat farPlane)
{
	const auto proj = glm::perspective(glm::radians(cam.zoom),
		1440.0f/800.0f, nearPlane, farPlane);
	const auto corners = getFrustrumCornersWorldSpace(proj * cam.getView());

	glm::vec3 center(0.0f);

	for (const auto& v : corners)
	{
		center += glm::vec3(v);
	}
	center /= corners.size();

	const auto lightView = glm::lookAt(center + Light_values::direct.direction, center,
		glm::vec3(0.0f, 1.0f, 0.0f));

	GLfloat minX = std::numeric_limits<float>::max();
	GLfloat maxX = std::numeric_limits<float>::lowest();
	GLfloat minY = std::numeric_limits<float>::max();
	GLfloat maxY = std::numeric_limits<float>::lowest();
	GLfloat minZ = std::numeric_limits<float>::max();
	GLfloat maxZ = std::numeric_limits<float>::lowest();
	for (const auto& v : corners)
	{
		const auto trf = lightView * v;
		minX = std::min(minX, trf.x);
		maxX = std::max(maxX, trf.x);
		minY = std::min(minY, trf.y);
		maxY = std::max(maxY, trf.y);
		minZ = std::min(minZ, trf.z);
		maxZ = std::max(maxZ, trf.z);
	}
	const float zMult = 10.0f;

	if (minZ < 0)
	{
		minZ *= zMult;
	}
	else
	{
		minZ /= zMult;
	}
	if (maxZ < 0)
	{
		maxZ /= zMult;
	}
	else
	{
		maxZ *= zMult;
	}
	
	const auto lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

	return lightProjection * lightView;
}

void generateLightFBOAndTex(GLuint& lightFBO, GLuint& lightFBOTex)
{
	glGenFramebuffers(1, &lightFBO);

	glGenTextures(1, &lightFBOTex);
	glBindTexture(GL_TEXTURE_2D_ARRAY, lightFBOTex);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, 4096, 4096, 
		(GLint)shadowCascadeLevels.size() + 1, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	constexpr GLfloat borderColor[] = {1.0f, 1.0f,1.0f,1.0f};
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightFBOTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";
		throw 0;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

namespace Shadow_values {
	std::vector<GLfloat> shadowCascadeLevels{500.0f/50.0f, 500.0f/25.0f, 500.0f/10.0f, 500.0f/2.0f};
	bool shadow{ true };
}

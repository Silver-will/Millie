#include "Texture.h"
#include "General_utility.h"
#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>



Texture::Texture(string texturePath, Format col, WrapM w)
{
	GLint width, height, nrchannels;
	glGenTextures(1, &this->TexID);
	glBindTexture(this->TexID, GL_TEXTURE_2D);
	switch (w)
	{
	case REPEAT:
		this->wrapping = GL_REPEAT;
		break;
	case MREPEAT:
		break;
		this->wrapping = GL_MIRRORED_REPEAT;
	case CLAMP_EDGE:
		break;
		this->wrapping = GL_CLAMP_TO_EDGE;
	case CLAMP_BORDER:
		break;
		this->wrapping = GL_CLAMP_TO_BORDER;
	default:
		Log("Wrapping Method not found");
		break;
	}

	switch (col)
	{
	case R:
		this->format = GL_RED;
	case RG:
		this->format = GL_RG;
	case RGB:
		this->format = GL_RGB;
	case RGBA:
		this->format = GL_RGBA;
	default:
		Log("Texture format specified not found");

	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrchannels, 0);
	
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, this->format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		Log("Failed to load texture data");
	}
	stbi_image_free(data);
}
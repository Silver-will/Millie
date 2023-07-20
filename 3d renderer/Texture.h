#ifndef TEXTURE_H
#define TEXTURE_H

#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<string>
#include"General_utility.h"
enum  WrapM
{
	REPEAT,
	MREPEAT,
	CLAMP_EDGE,
	CLAMP_BORDER
};
enum Format
{
	R,
	RG,
	RGB,
	RGBA
};
struct Texture
{
	//constructor taking a path, colour format and texture wrapping method to generate a texture object
	Texture(string texturePath, Format col, WrapM w);
	void setInt(GLint num);
	GLuint TexID;
	~Texture();
	GLuint format;
	GLuint wrapping;
};
#endif
#ifndef VAO_H
#define VAO_H
#include<GLAD/glad.h>
#include<GLFW/glfw3.h>
#include<map>
#include<vector>
#include<string>
struct VAO
{
	VAO();
	void GenerateBuffer(std::string type, std::vector<GLfloat>& vertices, std::vector<GLint>& offset);
	void bind();
	void unbind();
	~VAO();
private:
	GLuint ID;
	std::vector<std::pair<GLuint,std::string>> buffers;
};
#endif


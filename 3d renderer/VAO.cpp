#include "VAO.h"
#include<numeric>

VAO::VAO()
{
	glGenVertexArrays(1, &this->ID);
}

void VAO::bind()
{
	glBindVertexArray(this->ID);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}

void VAO::GenerateBuffer(std::string type, std::vector<GLfloat>& vertices, std::vector<GLint>& stride)
{
	std::pair<GLuint, std::string> hold{};
	auto& first = hold.first;
	hold.second = type;
	bind();
	if (type == "ELEMENT")
	{
		glGenBuffers(1, &first);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, first);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		this->buffers.push_back(hold);
	}
	if (type == "VERTEX")
	{
		glGenBuffers(1, &first);
		glBindBuffer(GL_ARRAY_BUFFER, first);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
		this->buffers.push_back(hold);
		GLint totalOff = 0;
		totalOff = std::accumulate(stride.begin(), stride.end(), 0);
		
		GLint offset{ 0 };
		for (size_t i = 0; i < stride.size(); i++)
		{
			glVertexAttribPointer(i, stride[i], GL_FLOAT, GL_FALSE, totalOff * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
			glEnableVertexAttribArray(i);
			offset += stride[i];
		}
	}

}
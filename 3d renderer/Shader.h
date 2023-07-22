#ifndef SHADER_H
#define SHADER_H
#include"General_utility.h"
#include<filesystem>
#include<string_view>
using std::string_view;
string loadFromFile(string_view shader);
struct Shader {
	Shader(string_view vertexPath, string_view fragmentPath);
	void    checkCompileErrors(GLuint object, string type);
	void    use();
    void    SetFloat(string name, float value, bool useShader = false);
    void    SetInteger(string name, int value, bool useShader = false);
    void    SetVector2f(string name, float x, float y, bool useShader = false);
    void    SetVector2f(string name, const glm::vec2& value, bool useShader = false);
    void    SetVector3f(string name, float x, float y, float z, bool useShader = false);
    void    SetVector3f(string name, const glm::vec3& value, bool useShader = false);
    void    SetVector4f(string name, float x, float y, float z, float w, bool useShader = false);
    void    SetVector4f(string name, const glm::vec4& value, bool useShader = false);
    void    SetMatrix4(string name, const glm::mat4& matrix, bool useShader = false);

	GLuint shad;
	~Shader();
};
#endif
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
    void    SetFloat(const char* name, float value, bool useShader = false);
    void    SetInteger(const char* name, int value, bool useShader = false);
    void    SetVector2f(const char* name, float x, float y, bool useShader = false);
    void    SetVector2f(const char* name, const glm::vec2& value, bool useShader = false);
    void    SetVector3f(const char* name, float x, float y, float z, bool useShader = false);
    void    SetVector3f(const char* name, const glm::vec3& value, bool useShader = false);
    void    SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
    void    SetVector4f(const char* name, const glm::vec4& value, bool useShader = false);
    void    SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false);

	GLuint shad;
	~Shader();
};
#endif
#include "Shader.h"
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

string loadFromFile(string_view shader)
{
    fstream shad;
    string code;
    shad.exceptions(std::fstream::failbit | std::fstream::badbit);
    try {
        shad.open(shader);
        stringstream hold;
        hold << shad.rdbuf();
        code = hold.str();
        shad.close();
    }
    catch (exception& e)
    {
        Log("Failed to load file");
    }
    return code;
    
}
Shader::Shader(string_view vertexPath, string_view fragmentPath)
{
    string vertex = loadFromFile(vertexPath);
    string fragment = loadFromFile(fragmentPath);
    const char* vertexcode = vertex.c_str();
    const char* fragmentcode = fragment.c_str();
    //ignore the geometry path since the renderer won't be using that
    GLuint vshader, fshader;
	vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, 1, &vertexcode, NULL);
    glCompileShader(vshader);
    checkCompileErrors(vshader, "vertex");

    fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, 1, &fragmentcode, NULL);
    glCompileShader(fshader);
    checkCompileErrors(fshader, "fragment");
    this->shad = glCreateProgram();
    glAttachShader(this->shad, vshader);
    glAttachShader(this->shad, fshader);
    
    glLinkProgram(this -> shad);
    checkCompileErrors(this->shad, "PROGRAM");
    glDeleteShader(vshader);
    glDeleteShader(fshader);
}

void Shader::checkCompileErrors(GLuint object, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                << infoLog << "\n -- --------------------------------------------------- -- "
                << std::endl;
        }
    }
}

void Shader::SetFloat(const char* name, float value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1f(glGetUniformLocation(this->shad, name), value);
}
void Shader::SetInteger(const char* name, int value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->shad, name), value);
}
void Shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->shad, name), x, y);
}
void Shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->shad, name), value.x, value.y);
}
void Shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->shad, name), x, y, z);
}
void Shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->shad, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->shad, name), x, y, z, w);
}
void Shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->shad, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
    if (useShader)
        this->use();
    glUniformMatrix4fv(glGetUniformLocation(this->shad, name), 1, false, glm::value_ptr(matrix));
}

void Shader::use()
{
    glUseProgram(this->shad);
}

Shader::~Shader()
{
    glDeleteProgram(this->shad);
}

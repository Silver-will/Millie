#include "Shader.h"
#include<iostream>
#include<fstream>
#include<sstream>
using namespace std;

string loadFromFile(string_view shader)
{
    fstream shade;
    string code;
    shade.exceptions(std::fstream::failbit | std::fstream::badbit);
    try {
        shade.open(shader);
        stringstream hold;
        hold << shade.rdbuf();
        code = hold.str();
        shade.close();
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

void Shader::SetFloat(string name, float value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1f(glGetUniformLocation(this->shad, name.c_str()), value);
}
void Shader::SetInteger(string name, int value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform1i(glGetUniformLocation(this->shad, name.c_str()), value);
}
void Shader::SetVector2f(string name, float x, float y, bool useShader)
{
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->shad, name.c_str()), x, y);
}
void Shader::SetVector2f(string name, const glm::vec2& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform2f(glGetUniformLocation(this->shad, name.c_str()), value.x, value.y);
}
void Shader::SetVector3f(string name, float x, float y, float z, bool useShader)
{
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->shad, name.c_str()), x, y, z);
}
void Shader::SetVector3f(string name, const glm::vec3& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform3f(glGetUniformLocation(this->shad, name.c_str()), value.x, value.y, value.z);
}
void Shader::SetVector4f(string name, float x, float y, float z, float w, bool useShader)
{
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->shad, name.c_str()), x, y, z, w);
}
void Shader::SetVector4f(string name, const glm::vec4& value, bool useShader)
{
    if (useShader)
        this->use();
    glUniform4f(glGetUniformLocation(this->shad, name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(string name, const glm::mat4& matrix, bool useShader)
{
    if (useShader)
        this->use();
    glUniformMatrix4fv(glGetUniformLocation(this->shad, name.c_str()), 1, false, glm::value_ptr(matrix));
}

void Shader::SetMatrix3(string name, const glm::mat3& matrix, bool useShader)
{
    if (useShader)
        this->use();
    glUniformMatrix3fv(glGetUniformLocation(this->shad, name.c_str()), 1, false, glm::value_ptr(matrix));
}

void Shader::use()
{
    glUseProgram(this->shad);
}

Shader::~Shader()
{
    glDeleteProgram(this->shad);
}

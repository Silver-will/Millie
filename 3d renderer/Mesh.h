#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

struct Texture {
    GLuint id;
    string type;
    string path;
};

struct Mesh {
    // mesh Data
    vector<Vertex>       vertices;
    vector<GLuint> indices;
    vector<Texture>      textures;
    GLuint VAO;

    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    void Draw(Shader& shader);

private:
    // render data 
    GLuint VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh();
};
#endif

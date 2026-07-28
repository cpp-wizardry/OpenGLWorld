//3DModelLoader.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <Windows.h>



struct OBJData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};
struct MeshData {
    std::vector<GLfloat> vertices;
};

class Manager3D
{

public:
    std::vector<MeshData> loadOBJ(const std::string& Path);
    std::vector<GLsizei> reloadModel(const std::string& path, std::vector<GLuint>& VBOs, std::vector<GLuint>& VAOs);
    GLuint reloadTexture(const std::string& path, GLuint oldTexture);
private:
    std::vector<OBJData> m_OBJData;
    std::string currOBJPath;



    void centerAndNormalizeOBJ(std::vector<GLfloat>& vertices);
};

GLuint loadTexture(const GLchar* filename);
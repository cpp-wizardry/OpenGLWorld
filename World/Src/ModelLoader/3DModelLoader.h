//3DModelLoader.h
#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "../../Includes/Mesh.h"

struct RawMeshData {
    std::vector<GLfloat> vertices; // pos3 + uv2 + normal3
};

class Manager3D
{

public:
    
    std::vector<Mesh> LoadModel(const std::string& path);
    GLuint reloadTexture(const std::string& path, GLuint oldTexture);


    
    std::vector<GLsizei> reloadModel(const std::string& path, std::vector<GLuint>& VBOs, std::vector<GLuint>& VAOs);
    
private:
    std::vector<RawMeshData> loadOBJ(const std::string& Path);
    void centerAndNormalizeOBJ(std::vector<GLfloat>& vertices);

};

GLuint loadTexture(const GLchar* filename);
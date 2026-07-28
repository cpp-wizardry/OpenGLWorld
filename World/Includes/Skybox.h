#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>
#include "../Src/ModelLoader/stb_image.h"
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Skybox {
public:
    explicit Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    Skybox(const Skybox&) = delete;
    Skybox& operator=(const Skybox&) = delete;

    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint cubemapTexture = 0;
    Mesh cubeMesh;
};

//Skybox.h
#include <glad/glad.h>
#include <string>
#include <vector>
#include "../Src/ModelLoader/stb_image.h"
#include "../Includes/Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Skybox {
public:
    unsigned int cubemapTexture;
    unsigned int VAO, VBO;

    Skybox(const std::vector<std::string>& faces);

    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

    ~Skybox();
};

#endif
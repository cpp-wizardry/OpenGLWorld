#pragma once
#include <glad/glad.h>  
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class BlackHole {

public:
    glm::vec3 Position;
    GLfloat SphereRadius;
    GLboolean Enabled = true;

    BlackHole(const glm::vec3& position, GLfloat sphereRadius = 1.5f, GLfloat diskInnerRadius = 2.2f, GLfloat diskOuterRadius = 4.5f, GLint sphereSectors = 36, GLint sphereStacks = 18, GLint diskSegments = 64);
    void Draw(Shader& sphereShader, Shader& diskShader, const glm::mat4& view, const glm::mat4& projection, GLfloat time) const;
    /*void toggleBlackHole() {
        if (currentFrame - lastToggle > 0.3f) {
            blackHole.Enabled = !blackHole.Enabled;
            lastToggle = currentFrame;
            std::cout << "Black hole " << (blackHole.Enabled ? "enabled" : "disabled") << "\n";
        }
    };*/
    
private:
    Mesh sphereMesh;
    Mesh diskMesh;
};
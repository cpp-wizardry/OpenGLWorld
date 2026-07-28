#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class BlackHole {
public:
    BlackHole(const glm::vec3& position,
        float sphereRadius = 1.5f,
        float diskInnerRadius = 2.2f,
        float diskOuterRadius = 4.5f,
        int sphereSectors = 36, int sphereStacks = 18,
        int diskSegments = 64);

    void Draw(Shader& sphereShader, Shader& diskShader,
        const glm::mat4& view, const glm::mat4& projection, float time) const;

    glm::vec3 Position;
    float SphereRadius;

private:
    Mesh sphereMesh;
    Mesh diskMesh;
};

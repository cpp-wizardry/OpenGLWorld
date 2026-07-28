#include "../../Includes/BlackHole.h"
#include "../../Includes/Primitives.h"
#include <glm/gtc/matrix_transform.hpp>

BlackHole::BlackHole(const glm::vec3& position, float sphereRadius,
    float diskInnerRadius, float diskOuterRadius,
    int sphereSectors, int sphereStacks, int diskSegments)
    : Position(position), SphereRadius(sphereRadius),
    sphereMesh([&] {
    auto data = Primitives::Sphere(sphereRadius, sphereSectors, sphereStacks);
    return Mesh(data.vertices, data.indices, { {0, 3} });
        }()),
    diskMesh([&] {
    auto data = Primitives::Ring(diskInnerRadius, diskOuterRadius, diskSegments);
    return Mesh(data.vertices, { {0, 3}, {1, 1} });
        }())
{
}

void BlackHole::Draw(Shader& sphereShader, Shader& diskShader,
    const glm::mat4& view, const glm::mat4& projection, float time) const {
    sphereShader.use();
    sphereShader.setMat4("view", view);
    sphereShader.setMat4("projection", projection);
    sphereShader.setMat4("model", glm::translate(glm::mat4(1.0f), Position));
    sphereMesh.Draw(GL_TRIANGLES);

    diskShader.use();
    diskShader.setMat4("view", view);
    diskShader.setMat4("projection", projection);
    glm::mat4 diskModel = glm::translate(glm::mat4(1.0f), Position);
    diskModel = glm::rotate(diskModel, time * 0.4f, glm::vec3(0, 1, 0));
    diskShader.setMat4("model", diskModel);
    diskMesh.Draw(GL_TRIANGLES);
}

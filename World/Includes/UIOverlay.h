#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class UIOverlay {
public:
    UIOverlay(float aspectRatio, float crosshairThickness = 0.005f, float crosshairLength = 0.05f);

    void SetAspectRatio(float aspectRatio);
    void Draw(Shader& uiShader, const glm::vec3& color) const;

private:
    Mesh swatchMesh;
    Mesh crosshairMesh;
    float thickness, length;
};

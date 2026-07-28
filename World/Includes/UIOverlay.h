#pragma once
#include "Mesh.h"
#include "Shader.h"
#include <glm/glm.hpp>

class UIOverlay {
public:
    UIOverlay(GLfloat aspectRatio, GLfloat crosshairThickness = 0.005f, GLfloat crosshairLength = 0.05f);

    void SetAspectRatio(GLfloat aspectRatio);
    void Draw(Shader& uiShader, const glm::vec3& color) const;

private:
    Mesh swatchMesh;
    Mesh crosshairMesh;
    GLfloat thickness, length;
};

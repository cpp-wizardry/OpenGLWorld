#include "../Includes/UIOverlay.h"
#include "../Includes/Primitives.h"

namespace {
    Mesh MakeSwatch() {
        auto data = Primitives::RectQuad(-0.95f, 0.75f, -0.75f, 0.95f);
        return Mesh(data.vertices, { {0, 2} });
    }
    Mesh MakeCrosshair(GLfloat aspect, GLfloat thickness, GLfloat length) {
        auto data = Primitives::Crosshair(aspect, thickness, length);
        return Mesh(data.vertices, { {0, 2} });
    }
}

UIOverlay::UIOverlay(GLfloat aspectRatio, GLfloat crosshairThickness, GLfloat crosshairLength)
    : swatchMesh(MakeSwatch()),
    crosshairMesh(MakeCrosshair(aspectRatio, crosshairThickness, crosshairLength)),
    thickness(crosshairThickness), length(crosshairLength)
{
}

void UIOverlay::SetAspectRatio(GLfloat aspectRatio) {
    crosshairMesh = MakeCrosshair(aspectRatio, thickness, length);
}

void UIOverlay::Draw(Shader& uiShader, const glm::vec3& color) const {
    uiShader.use();
    uiShader.setVec3("color", color);

    swatchMesh.Draw(GL_TRIANGLES);
    crosshairMesh.Draw(GL_TRIANGLES);
}

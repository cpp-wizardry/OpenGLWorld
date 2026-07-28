#pragma once
#include <vector>
#include <glad/glad.h>

namespace Primitives {

    struct MeshData {
        std::vector<GLfloat> vertices;
        std::vector<GLuint> indices;
    };

    MeshData Sphere(GLfloat radius, GLint sectors, GLint stacks);

    MeshData Ring(GLfloat innerRadius, GLfloat outerRadius, GLint segments);

    MeshData Plane(GLfloat width, GLfloat height, GLfloat uvTileSize = 1.0f);

    MeshData FullscreenQuad();

    MeshData RectQuad(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);

    MeshData Crosshair(GLfloat aspectRatio, GLfloat thickness, GLfloat length);
}

#pragma once
#include <vector>

namespace Primitives {

    struct MeshData {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;
    };

    MeshData Sphere(float radius, int sectors, int stacks);

    MeshData Ring(float innerRadius, float outerRadius, int segments);

    MeshData Plane(float width, float height);

    MeshData FullscreenQuad();

    MeshData RectQuad(float x0, float y0, float x1, float y1);

    MeshData Crosshair(float aspectRatio, float thickness, float length);

}

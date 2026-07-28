#include "../../Includes/Primitives.h"
#include <glm/glm.hpp>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>


namespace Primitives {

    MeshData Sphere(float radius, int sectors, int stacks) {
        MeshData data;

        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = glm::pi<float>() / 2.0f - i * (glm::pi<float>() / stacks);
            float xy = radius * cosf(stackAngle);
            float z = radius * sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j) {
                float sectorAngle = j * (2.0f * glm::pi<float>() / sectors);
                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);
                data.vertices.push_back(x);
                data.vertices.push_back(z);
                data.vertices.push_back(y);
            }
        }

        for (int i = 0; i < stacks; ++i) {
            int k1 = i * (sectors + 1);
            int k2 = k1 + sectors + 1;

            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    data.indices.push_back(k1);
                    data.indices.push_back(k2);
                    data.indices.push_back(k1 + 1);
                }
                if (i != (stacks - 1)) {
                    data.indices.push_back(k1 + 1);
                    data.indices.push_back(k2);
                    data.indices.push_back(k2 + 1);
                }
            }
        }

        return data;
    }

    MeshData Ring(float innerRadius, float outerRadius, int segments) {
        MeshData data;

        for (int i = 0; i < segments; ++i) {
            float a0 = i * (2.0f * glm::pi<float>() / segments);
            float a1 = (i + 1) * (2.0f * glm::pi<float>() / segments);

            glm::vec2 innerA(cosf(a0) * innerRadius, sinf(a0) * innerRadius);
            glm::vec2 innerB(cosf(a1) * innerRadius, sinf(a1) * innerRadius);
            glm::vec2 outerA(cosf(a0) * outerRadius, sinf(a0) * outerRadius);
            glm::vec2 outerB(cosf(a1) * outerRadius, sinf(a1) * outerRadius);

            auto push = [&](glm::vec2 p, float dist) {
                data.vertices.push_back(p.x);
                data.vertices.push_back(0.0f);
                data.vertices.push_back(p.y);
                data.vertices.push_back(dist);
                };

            push(innerA, 0.0f); push(outerA, 1.0f); push(outerB, 1.0f);
            push(innerA, 0.0f); push(outerB, 1.0f); push(innerB, 0.0f);
        }

        return data;
    }

    MeshData Plane(float width, float height) {
        float nx = 0.0f, ny = 1.0f, nz = 0.0f;
        MeshData data;
        data.vertices = {
            0,      0,      0,    nx,ny,nz,     0,0,
            width,  0,      0,    nx,ny,nz,     1,0,
            width,  0,  height,   nx,ny,nz,     1,1,
            0,      0,  height,   nx,ny,nz,     0,1
        };
        return data;
    }

    MeshData FullscreenQuad() {
        MeshData data;
        data.vertices = {
            -1.0f,  1.0f,   0.0f, 1.0f,
            -1.0f, -1.0f,   0.0f, 0.0f,
             1.0f, -1.0f,   1.0f, 0.0f,

            -1.0f,  1.0f,   0.0f, 1.0f,
             1.0f, -1.0f,   1.0f, 0.0f,
             1.0f,  1.0f,   1.0f, 1.0f
        };
        return data;
    }

    MeshData RectQuad(float x0, float y0, float x1, float y1) {
        MeshData data;
        data.vertices = {
            x0, y1,
            x0, y0,
            x1, y0,

            x0, y1,
            x1, y0,
            x1, y1
        };
        return data;
    }

    MeshData Crosshair(float aspectRatio, float thickness, float length) {
        float vBarHalfW = thickness / aspectRatio;
        float hBarHalfH = thickness;

        MeshData data;
        data.vertices = {
            -vBarHalfW, -length,
             vBarHalfW, -length,
             vBarHalfW,  length,

            -vBarHalfW, -length,
             vBarHalfW,  length,
            -vBarHalfW,  length,

            -length, -hBarHalfH,
             length, -hBarHalfH,
             length,  hBarHalfH,

            -length, -hBarHalfH,
             length,  hBarHalfH,
            -length,  hBarHalfH
        };
        return data;
    }

}

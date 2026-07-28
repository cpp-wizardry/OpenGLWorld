#include "../../Includes/Primitives.h"
#include <glm/glm.hpp>
#include <cmath>
#include <glm/ext/scalar_constants.hpp>


namespace Primitives {

    MeshData Sphere(GLfloat radius, GLint sectors, GLint stacks) {
        MeshData data;

        for (GLint i = 0; i <= stacks; ++i) {
            GLfloat stackAngle = glm::pi<GLfloat>() / 2.0f - i * (glm::pi<GLfloat>() / stacks);
            GLfloat xy = radius * cosf(stackAngle);
            GLfloat z = radius * sinf(stackAngle);

            for (GLint j = 0; j <= sectors; ++j) {
                GLfloat sectorAngle = j * (2.0f * glm::pi<GLfloat>() / sectors);
                GLfloat x = xy * cosf(sectorAngle);
                GLfloat y = xy * sinf(sectorAngle);
                data.vertices.push_back(x);
                data.vertices.push_back(z);
                data.vertices.push_back(y);
            }
        }

        for (GLint i = 0; i < stacks; ++i) {
            GLint k1 = i * (sectors + 1);
            GLint k2 = k1 + sectors + 1;

            for (GLint j = 0; j < sectors; ++j, ++k1, ++k2) {
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

    MeshData Ring(GLfloat innerRadius, GLfloat outerRadius, GLint segments) {
        MeshData data;

        for (GLint i = 0; i < segments; ++i) {
            GLfloat a0 = i * (2.0f * glm::pi<GLfloat>() / segments);
            GLfloat a1 = (i + 1) * (2.0f * glm::pi<GLfloat>() / segments);

            glm::vec2 innerA(cosf(a0) * innerRadius, sinf(a0) * innerRadius);
            glm::vec2 innerB(cosf(a1) * innerRadius, sinf(a1) * innerRadius);
            glm::vec2 outerA(cosf(a0) * outerRadius, sinf(a0) * outerRadius);
            glm::vec2 outerB(cosf(a1) * outerRadius, sinf(a1) * outerRadius);

            auto push = [&](glm::vec2 p, GLfloat dist) {
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

    MeshData Plane(GLfloat width, GLfloat height,GLfloat uvTileSize) {
        GLfloat nx = 0.0f, ny = 1.0f, nz = 0.0f;

        GLfloat u = width / uvTileSize;
        GLfloat v = height / uvTileSize;
        MeshData data;
        data.vertices = {
            0,      0,      0,    nx,ny,nz,     0,0,
            width,  0,      0,    nx,ny,nz,     u,0,
            width,  0,  height,   nx,ny,nz,     u,v,
            0,      0,  height,   nx,ny,nz,     0,v
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

    MeshData RectQuad(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1) {
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

    MeshData Crosshair(GLfloat aspectRatio, GLfloat thickness, GLfloat length) {
        GLfloat vBarHalfW = thickness / aspectRatio;
        GLfloat hBarHalfH = thickness;

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

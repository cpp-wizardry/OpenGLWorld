#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <limits>
#include "Mesh.h"
#include "Shader.h"

class PointBuilder {
public:
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;

    std::vector<int> activeChain;

    void AddPoint(const glm::vec3& pos, const glm::vec3& color) {
        int newIndex = static_cast<int>(points.size());
        points.push_back(pos);
        colors.push_back(color);

        if (activeChain.size() >= 2) {
            int nearest1 = -1, nearest2 = -1;
            float d1 = std::numeric_limits<float>::max();
            float d2 = std::numeric_limits<float>::max();

            for (int idx : activeChain) {
                float d = glm::length(points[idx] - pos);
                if (d < d1) {
                    d2 = d1; nearest2 = nearest1;
                    d1 = d;  nearest1 = idx;
                }
                else if (d < d2) {
                    d2 = d; nearest2 = idx;
                }
            }

            if (nearest1 >= 0 && nearest2 >= 0) {
                indices.push_back(static_cast<unsigned int>(newIndex));
                indices.push_back(static_cast<unsigned int>(nearest1));
                indices.push_back(static_cast<unsigned int>(nearest2));
            }
        }

        activeChain.push_back(newIndex);
        dirty = true;
    }

    void ResetChain() {
        activeChain.clear();
    }

    int FindNearestPoint(const glm::vec3& pos, float radius) const {
        int best = -1;
        float bestDist = radius;
        for (size_t i = 0; i < points.size(); i++) {
            float d = glm::length(points[i] - pos);
            if (d < bestDist) { bestDist = d; best = static_cast<int>(i); }
        }
        return best;
    }

    void Clear() {
        points.clear();
        colors.clear();
        indices.clear();
        activeChain.clear();
        dirty = true;
    }

    void DrawTriangles(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        Update();
        if (indices.empty()) return;
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.Draw(GL_TRIANGLES);
    }

    void DrawPoints(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        Update();
        if (points.empty()) return;
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        mesh.Draw(GL_POINTS, false);
    }

private:
    void Update() {
        if (!dirty) return;
        std::vector<float> interleaved;
        interleaved.reserve(points.size() * 6);
        for (size_t i = 0; i < points.size(); i++) {
            interleaved.push_back(points[i].x);
            interleaved.push_back(points[i].y);
            interleaved.push_back(points[i].z);
            interleaved.push_back(colors[i].r);
            interleaved.push_back(colors[i].g);
            interleaved.push_back(colors[i].b);
        }
        if (!mesh.IsValid()) {
            mesh = Mesh(interleaved, { {0, 3}, {1, 3} }, GL_DYNAMIC_DRAW);
        }
        else {
            mesh.UpdateVertices(interleaved);
        }
        if (!indices.empty()) mesh.UpdateIndices(indices);
        dirty = false;
    }
    Mesh mesh;
    bool dirty = true;
};
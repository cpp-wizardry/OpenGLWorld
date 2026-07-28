#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Mesh.h"
#include "Shader.h"

class PointBuilder {
public:
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    std::vector<unsigned int> indices;
    std::vector<int> pending;

    void AddPoint(const glm::vec3& pos, const glm::vec3& color) {
        points.push_back(pos);
        colors.push_back(color);
        dirty = true;
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

    void SelectPoint(int index) {
        pending.push_back(index);
        if (pending.size() == 3) {
            indices.push_back(pending[0]);
            indices.push_back(pending[1]);
            indices.push_back(pending[2]);
            pending.clear();
            dirty = true;
        }
    }

    void Clear() {
        points.clear();
        colors.clear();
        indices.clear();
        pending.clear();
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

//PointBuilder.h
#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include "Camera.h"
#include "Shader.h"

class PointBuilder {
public:
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    std::vector<GLuint> indices;
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

    void Update() {
        if (!dirty) return;
        if (VAO == 0) {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
        }
        glBindVertexArray(VAO);

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

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        if (!interleaved.empty()) {
            glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(float),
                interleaved.data(), GL_DYNAMIC_DRAW);
        }
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        if (!indices.empty()) {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                indices.data(), GL_DYNAMIC_DRAW);
        }

        glBindVertexArray(0);
        dirty = false;
    }

    void DrawTriangles(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        Update();
        if (indices.empty()) return;
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void DrawPoints(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
        Update();
        if (points.empty()) return;
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
        glBindVertexArray(0);
    }

private:
    GLuint VAO = 0, VBO = 0, EBO = 0;
    bool dirty = true;
};

inline glm::vec3 GetClickPoint(const glm::vec3& origin, const glm::vec3& dir, float maxRange)
{
    glm::vec3 hit;
    bool hasHit = false;

    if (std::fabs(dir.y) > 1e-5f) {
        float t = -origin.y / dir.y;
        if (t > 0.0f) {
            hit = origin + dir * t;
            hasHit = true;
        }
    }

    if (!hasHit) {
        hit = origin + dir * maxRange;
    }

    float dist = glm::length(hit - origin);
    if (dist > maxRange) {
        hit = origin + dir * maxRange;
    }

    return hit;
}

inline glm::vec3 HueToRGB(float hueDegrees)
{
    float h = hueDegrees / 60.0f;
    float c = 1.0f;
    float x = c * (1.0f - std::fabs(std::fmod(h, 2.0f) - 1.0f));

    glm::vec3 rgb;
    if (h < 1.0f)      rgb = glm::vec3(c, x, 0.0f);
    else if (h < 2.0f) rgb = glm::vec3(x, c, 0.0f);
    else if (h < 3.0f) rgb = glm::vec3(0.0f, c, x);
    else if (h < 4.0f) rgb = glm::vec3(0.0f, x, c);
    else if (h < 5.0f) rgb = glm::vec3(x, 0.0f, c);
    else               rgb = glm::vec3(c, 0.0f, x);
    return rgb;
}
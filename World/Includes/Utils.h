#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <cmath>
#include "Camera.h"

namespace Utils {

    inline glm::vec3 GetClickPoint(const glm::vec3& origin, const glm::vec3& dir, GLfloat maxRange) {
        glm::vec3 hit;
        GLboolean hasHit = false;

        if (std::fabs(dir.y) > 1e-5f) {
            GLfloat t = -origin.y / dir.y;
            if (t > 0.0f) {
                hit = origin + dir * t;
                hasHit = true;
            }
        }

        if (!hasHit) {
            hit = origin + dir * maxRange;
        }

        if (glm::length(hit - origin) > maxRange) {
            hit = origin + dir * maxRange;
        }

        return hit;
    }

    inline glm::vec3 HueToRGB(GLfloat hueDegrees) {
        GLfloat h = hueDegrees / 60.0f;
        GLfloat c = 1.0f;
        GLfloat x = c * (1.0f - std::fabs(std::fmod(h, 2.0f) - 1.0f));

        glm::vec3 rgb;
        if (h < 1.0f)      rgb = glm::vec3(c, x, 0.0f);
        else if (h < 2.0f) rgb = glm::vec3(x, c, 0.0f);
        else if (h < 3.0f) rgb = glm::vec3(0.0f, c, x);
        else if (h < 4.0f) rgb = glm::vec3(0.0f, x, c);
        else if (h < 5.0f) rgb = glm::vec3(x, 0.0f, c);
        else               rgb = glm::vec3(c, 0.0f, x);
        return rgb;
    }

    struct LensingData {
        glm::vec2 screenUV = glm::vec2(0.5f);
        GLfloat horizonRadiusUV = 0.05f;
        GLboolean visible = false;
    };

    inline LensingData ComputeLensingData(const glm::vec3& sphereWorldPos, GLfloat sphereRadius,
        const Camera& camera, const glm::mat4& view, const glm::mat4& projection) {
        LensingData result;

        glm::vec4 clipPos = projection * view * glm::vec4(sphereWorldPos, 1.0f);
        if (clipPos.w <= 0.0f) return result;

        result.visible = true;
        glm::vec3 ndc = glm::vec3(clipPos) / clipPos.w;
        result.screenUV = glm::vec2(ndc.x, ndc.y) * 0.5f + 0.5f;

        glm::vec3 edgeWorldPos = sphereWorldPos + camera.Right * sphereRadius;
        glm::vec4 edgeClip = projection * view * glm::vec4(edgeWorldPos, 1.0f);
        if (edgeClip.w > 0.0f) {
            glm::vec3 edgeNdc = glm::vec3(edgeClip) / edgeClip.w;
            glm::vec2 edgeUV = glm::vec2(edgeNdc.x, edgeNdc.y) * 0.5f + 0.5f;
            result.horizonRadiusUV = glm::length(edgeUV - result.screenUV);
        }

        return result;
    }

}
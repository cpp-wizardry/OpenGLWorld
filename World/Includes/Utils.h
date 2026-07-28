#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include "Camera.h"

namespace Utils {

    inline glm::vec3 GetClickPoint(const glm::vec3& origin, const glm::vec3& dir, float maxRange) {
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

        if (glm::length(hit - origin) > maxRange) {
            hit = origin + dir * maxRange;
        }

        return hit;
    }

    inline glm::vec3 HueToRGB(float hueDegrees) {
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

    struct LensingData {
        glm::vec2 screenUV = glm::vec2(0.5f);
        float horizonRadiusUV = 0.05f;
    };

    inline LensingData ComputeLensingData(const glm::vec3& sphereWorldPos, float sphereRadius,
        const Camera& camera, const glm::mat4& view, const glm::mat4& projection) {
        LensingData result;

        glm::vec4 clipPos = projection * view * glm::vec4(sphereWorldPos, 1.0f);
        if (clipPos.w <= 0.0f) return result;

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

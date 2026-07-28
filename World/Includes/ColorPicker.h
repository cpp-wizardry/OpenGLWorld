#pragma once
#include "Utils.h"

class ColorPicker {
public:
    void OnScroll(double yoffset) {
        hue += static_cast<float>(yoffset) * degreesPerScroll;
        if (hue < 0.0f) hue += 360.0f;
        hue = std::fmod(hue, 360.0f);
    }

    glm::vec3 CurrentColor() const { return Utils::HueToRGB(hue); }

private:
    float hue = 0.0f;
    float degreesPerScroll = 15.0f;
};

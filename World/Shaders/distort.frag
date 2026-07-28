#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D sceneTexture;
uniform vec2 blackHoleScreenPos;
uniform float horizonRadius;
uniform float lensStrength;
uniform bool enabled;

void main() {
    if (!enabled) {
        FragColor = texture(sceneTexture, vUV);
        return;
    }

    vec2 toCenter = vUV - blackHoleScreenPos;
    float dist = length(toCenter);
    if (dist < horizonRadius) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }
    float falloff = lensStrength / (dist * dist + 0.01);
    vec2 warpedUV = vUV - normalize(toCenter) * falloff * 0.02;
    warpedUV = clamp(warpedUV, 0.0, 1.0);
    FragColor = texture(sceneTexture, warpedUV);
}
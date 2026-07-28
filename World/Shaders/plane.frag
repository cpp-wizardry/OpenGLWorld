#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 viewPos;

const vec3 colorA = vec3(0.16, 0.16, 0.18);
const vec3 colorB = vec3(0.24, 0.24, 0.27);

const vec3 fogColor = vec3(0.05, 0.05, 0.08);
const float fogDensity = 0.035;

void main() {
    float checker = mod(floor(TexCoord.x) + floor(TexCoord.y), 2.0);
    vec3 groundColor = mix(colorA, colorB, checker);

    vec3 lightDir = normalize(vec3(-0.2, -1.0, -0.3));
    float diff = max(dot(normalize(Normal), -lightDir), 0.0);
    vec3 lit = groundColor * (0.5 + 0.5 * diff);

    float dist = length(viewPos - FragPos);
    float fogFactor = 1.0 - exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(lit, fogColor, fogFactor);
    FragColor = vec4(finalColor, 1.0);
}

#version 330 core
in float vDist;
out vec4 FragColor;

void main() {
    vec3 hot = vec3(1.0, 0.95, 0.8);
    vec3 cool = vec3(0.9, 0.25, 0.05);
    vec3 color = mix(hot, cool, vDist);
    FragColor = vec4(color, 1.0);
}
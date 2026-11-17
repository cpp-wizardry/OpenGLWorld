#version 330 core
in vec3 vColor;
in float vFade;
out vec4 FragColor;

void main() {
    vec2 c = gl_PointCoord - vec2(0.5);
    if (length(c) > 0.5) discard;
    FragColor = vec4(vColor, vFade);
}
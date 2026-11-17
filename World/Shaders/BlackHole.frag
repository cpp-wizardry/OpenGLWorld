#version 330 core
in vec3 vLocalPos;
out vec4 FragColor;

uniform float time;
uniform float innerRadius;
uniform float outerRadius;
uniform int   ringCount;

void main() {
    float r = length(vLocalPos.xz);
    float t = clamp((r - innerRadius) / (outerRadius - innerRadius), 0.0, 1.0);
    float angle = atan(vLocalPos.z, vLocalPos.x);

    float wave = sin(t * float(ringCount) * 6.28318 - time * 1.5 + sin(angle * 3.0 + time) * 0.3);
    float band = smoothstep(0.2, 0.9, 0.5 + 0.5 * wave);   

    vec3 black = vec3(0.0);
    vec3 deepBlue = vec3(0.05, 0.25, 1.0);
    vec3 glowBlue = vec3(0.5, 0.85, 1.0);

    vec3 blue = mix(deepBlue, glowBlue, pow(band, 3.0));
    vec3 color = mix(black, blue, band);

    color *= mix(1.6, 0.6, t);
    float edgeFade = smoothstep(0.0, 0.08, t) * (1.0 - smoothstep(0.85, 1.0, t));

    float alpha = edgeFade * mix(0.9, 1.0, band);
    FragColor = vec4(color, alpha);
}
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

uniform vec3 viewPos;
uniform vec3 lightDir;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);


    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDirection + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * vec3(0.6);
    vec3 ambient = vec3(0.15);

    vec3 result = ((ambient + diffuse + specular) * 0.8) * vec3(0.9, 0.92, 1.0);
    FragColor = vec4(result, 1.0);
}
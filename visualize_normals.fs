#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D gNormal;

void main()
{
    vec3 normal = texture(gNormal, TexCoords).rgb;
    // Map from [-1,1] to [0,1] for visualization
    normal = normalize(normal) * 0.5 + 0.5;
    FragColor = vec4(normal, 1.0);
} 
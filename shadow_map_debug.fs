#version 330 core
in vec2 TexCoords;
out vec4 FragColor;
uniform sampler2D shadowMap;
void main() {
    float depth = texture(shadowMap, TexCoords).r;
    FragColor = vec4(vec3(depth), 1.0);
} 
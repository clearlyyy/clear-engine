#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D ssaoTex;
void main()
{
    float ssao = texture(ssaoTex, TexCoords).r;
    FragColor = vec4(ssao, ssao, ssao, 1.0);
} 
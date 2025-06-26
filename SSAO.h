#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"
#include "GBuffer.h"

class SSAO {
public:
    SSAO(unsigned int width, unsigned int height);
    ~SSAO();

    void generateKernel();
    void generateNoiseTexture();
    void resize(unsigned int width, unsigned int height);
    void renderSSAO(const GBuffer& gbuffer, const glm::mat4& projection, Shader& ssaoShader, GLuint quadVAO);
    void blurSSAO(Shader& blurShader, GLuint quadVAO);

    GLuint getSSAOTexture() const { return ssaoBlurColorBuffer; }

    float radius = 1.0f;
    float bias = 0.1f;
    float strength = 1.0f;

private:
    unsigned int width, height;
    GLuint ssaoFBO, ssaoColorBuffer;
    GLuint ssaoBlurFBO, ssaoBlurColorBuffer;
    GLuint ssaoNoiseTexture;
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    void init();
    void cleanup();
}; 
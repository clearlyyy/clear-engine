#include "SSAO.h"
#include <random>
#include <iostream>

SSAO::SSAO(unsigned int width, unsigned int height)
    : width(width), height(height), ssaoFBO(0), ssaoColorBuffer(0), ssaoBlurFBO(0), ssaoBlurColorBuffer(0), ssaoNoiseTexture(0)
{
    generateKernel();
    generateNoiseTexture();
    init();
}

SSAO::~SSAO() {
    cleanup();
}

void SSAO::generateKernel() {
    ssaoKernel.clear();
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 64; ++i) {
        glm::vec3 sample(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator)
        );
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        float scale = float(i) / 64.0f;
        scale = 0.1f + 0.9f * (scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }
}

void SSAO::generateNoiseTexture() {
    ssaoNoise.clear();
    std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
    std::default_random_engine generator;
    for (unsigned int i = 0; i < 16; i++) {
        glm::vec3 noise(
            randomFloats(generator) * 2.0 - 1.0,
            randomFloats(generator) * 2.0 - 1.0,
            0.0f);
        ssaoNoise.push_back(noise);
    }
    if (ssaoNoiseTexture) glDeleteTextures(1, &ssaoNoiseTexture);
    glGenTextures(1, &ssaoNoiseTexture);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAO::init() {
    // SSAO framebuffer
    glGenFramebuffers(1, &ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // SSAO blur framebuffer
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::cleanup() {
    if (ssaoColorBuffer) glDeleteTextures(1, &ssaoColorBuffer);
    if (ssaoBlurColorBuffer) glDeleteTextures(1, &ssaoBlurColorBuffer);
    if (ssaoNoiseTexture) glDeleteTextures(1, &ssaoNoiseTexture);
    if (ssaoFBO) glDeleteFramebuffers(1, &ssaoFBO);
    if (ssaoBlurFBO) glDeleteFramebuffers(1, &ssaoBlurFBO);
}

void SSAO::resize(unsigned int newWidth, unsigned int newHeight) {
    if (width == newWidth && height == newHeight) return;
    cleanup();
    width = newWidth;
    height = newHeight;
    init();
    generateNoiseTexture();
}

void SSAO::renderSSAO(const GBuffer& gbuffer, const glm::mat4& projection, Shader& ssaoShader, GLuint quadVAO) {
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    ssaoShader.use();
    for (unsigned int i = 0; i < 64; ++i)
        ssaoShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
    ssaoShader.setMat4("projection", projection);
    ssaoShader.setVec2("noiseScale", glm::vec2(width / 4.0f, height / 4.0f));
    ssaoShader.setInt("gPosition", 0);
    ssaoShader.setInt("gNormal", 1);
    ssaoShader.setInt("texNoise", 2);
    ssaoShader.setFloat("radius", radius);
    ssaoShader.setFloat("bias", bias);
    ssaoShader.setFloat("strength", strength);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gbuffer.getPositionTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gbuffer.getNormalTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, ssaoNoiseTexture);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SSAO::blurSSAO(Shader& blurShader, GLuint quadVAO) {
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    blurShader.use();
    blurShader.setInt("ssaoInput", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
} 
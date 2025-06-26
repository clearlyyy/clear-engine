#include "ShadowMap.h"
#include "Model.h"
#include <iostream>

ShadowMap::ShadowMap(unsigned int width, unsigned int height)
    : width(width), height(height), depthMapFBO(0), depthMap(0), blurredDepthMapFBO(0), blurredDepthMap(0),
      lightDirection(0.0f, -1.0f, 0.0f), lightPosition(0.0f), target(0.0f),
      projLeft(-10.0f), projRight(10.0f), projBottom(-10.0f), projTop(10.0f), projNear(0.001f), projFar(100.0f)
{
    init();
    updateMatrices();
}

ShadowMap::~ShadowMap() {
    cleanup();
}

void ShadowMap::init() {
    // Create depth map texture
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    
    // Set border color to prevent shadow acne
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    
    // Create framebuffer
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    
    // Disable color buffer for shadow mapping
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::SHADOWMAP:: Framebuffer is not complete!" << std::endl;
    }
    
    // Create blurred depth map texture
    glGenTextures(1, &blurredDepthMap);
    glBindTexture(GL_TEXTURE_2D, blurredDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Create blur framebuffer
    glGenFramebuffers(1, &blurredDepthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurredDepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurredDepthMap, 0);
    
    // Check blur framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::SHADOWMAP:: Blur framebuffer is not complete!" << std::endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::cleanup() {
    if (depthMapFBO) {
        glDeleteFramebuffers(1, &depthMapFBO);
        depthMapFBO = 0;
    }
    if (depthMap) {
        glDeleteTextures(1, &depthMap);
        depthMap = 0;
    }
    if (blurredDepthMapFBO) {
        glDeleteFramebuffers(1, &blurredDepthMapFBO);
        blurredDepthMapFBO = 0;
    }
    if (blurredDepthMap) {
        glDeleteTextures(1, &blurredDepthMap);
        blurredDepthMap = 0;
    }
}

void ShadowMap::beginShadowPass() {
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Configure OpenGL for shadow pass
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_CCW); // Cull front faces to reduce shadow acne
}

void ShadowMap::endShadowPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK); // Restore back face culling
}

void ShadowMap::renderModels(const std::vector<Model*>& models, Shader& shadowShader) {
    shadowShader.use();
    shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    
    for (const Model* model : models) {
        // Skip light models (they don't cast shadows)
        if (model->name.find("Light") != std::string::npos) {
            continue;
        }
        
        glm::mat4 modelMatrix = model->getTransform();
        shadowShader.setMat4("model", modelMatrix);
        
        // Render each mesh in the model
        for (const Mesh& mesh : model->getMeshes()) {
            mesh.drawShadowPass(shadowShader);
        }
    }
}

void ShadowMap::setLightDirection(const glm::vec3& direction) {
    lightDirection = glm::normalize(direction);
    updateMatrices();
}

void ShadowMap::setLightPosition(const glm::vec3& position) {
    lightPosition = position;
    updateMatrices();
}

void ShadowMap::setTarget(const glm::vec3& targetPos) {
    target = targetPos;
    updateMatrices();
}

void ShadowMap::setLightProjection(float left, float right, float bottom, float top, float near, float far) {
    projLeft = left;
    projRight = right;
    projBottom = bottom;
    projTop = top;
    projNear = near;
    projFar = far;
    updateMatrices();
}

void ShadowMap::updateLightSpaceMatrix() {
    updateMatrices();
}

void ShadowMap::updateMatrices() {
    // Create orthographic projection for directional light
    lightProjection = glm::ortho(projLeft, projRight, projBottom, projTop, projNear, projFar);
    
    // Create view matrix looking from light position to target
    lightView = glm::lookAt(lightPosition, target, glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Combine projection and view matrices
    lightSpaceMatrix = lightProjection * lightView;
}

void ShadowMap::resize(unsigned int newWidth, unsigned int newHeight) {
    if (width == newWidth && height == newHeight) return;
    
    cleanup();
    width = newWidth;
    height = newHeight;
    init();
}

void ShadowMap::bindDepthMap(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

void ShadowMap::bindBlurredDepthMap(GLuint textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, blurredDepthMap);
}

void ShadowMap::blurShadowMap(Shader& blurShader, GLuint quadVAO) {
    glBindFramebuffer(GL_FRAMEBUFFER, blurredDepthMapFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    
    blurShader.use();
    blurShader.setInt("shadowMap", 0);
    blurShader.setVec2("texelSize", glm::vec2(1.0f / width, 1.0f / height));
    blurShader.setInt("blurRadius", 1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::setupForDirectionalLight(const glm::vec3& direction, const glm::vec3& target, float distance) {
    setLightDirection(direction);
    setLightPosition(-glm::normalize(direction) * distance);
    setTarget(target);
    
    // Set reasonable orthographic projection bounds for directional light
    float size = distance * 10.0f;
    setLightProjection(-size, size, -size, size, 0.01f, distance * 50.0f);
}

void ShadowMap::setupForSpotLight(const glm::vec3& position, const glm::vec3& direction, float fov, float near, float far) {
    setLightPosition(position);
    setLightDirection(direction);
    setTarget(position + direction);
    
    // For spot lights, we use perspective projection
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    lightProjection = glm::perspective(glm::radians(fov), aspect, near, far);
    lightView = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
} 
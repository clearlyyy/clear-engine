#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shader.h"

// Forward declarations
class Model;

class ShadowMap {
public:
    ShadowMap(unsigned int width, unsigned int height);
    ~ShadowMap();

    // Core shadow mapping functionality
    void beginShadowPass();
    void endShadowPass();
    void renderModels(const std::vector<Model*>& models, Shader& shadowShader);
    void blurShadowMap(Shader& blurShader, GLuint quadVAO);
    
    // Light configuration
    void setLightDirection(const glm::vec3& direction);
    void setLightPosition(const glm::vec3& position);
    void setTarget(const glm::vec3& target);
    void setLightProjection(float left, float right, float bottom, float top, float near, float far);
    void updateLightSpaceMatrix();
    
    // Getters
    GLuint getDepthMap() const { return depthMap; }
    GLuint getBlurredDepthMap() const { return blurredDepthMap; }
    glm::mat4 getLightSpaceMatrix() const { return lightSpaceMatrix; }
    glm::mat4 getLightProjectionMatrix() const { return lightProjection; }
    glm::mat4 getLightViewMatrix() const { return lightView; }
    
    // Utility
    void resize(unsigned int width, unsigned int height);
    void bindDepthMap(GLuint textureUnit = 0) const;
    void bindBlurredDepthMap(GLuint textureUnit = 0) const;
    
    // Convenience methods for common light setups
    void setupForDirectionalLight(const glm::vec3& direction, const glm::vec3& target = glm::vec3(0.0f), float distance = 10.0f);
    void setupForSpotLight(const glm::vec3& position, const glm::vec3& direction, float fov = 45.0f, float near = 0.1f, float far = 100.0f);
    
    // Configuration
    float shadowBias = 0.005f;
    float shadowBiasSlope = 0.05f;
    bool enablePCF = true;
    int pcfKernelSize = 3; // 3x3 PCF kernel

private:
    unsigned int width, height;
    GLuint depthMapFBO;
    GLuint depthMap;
    GLuint blurredDepthMapFBO;
    GLuint blurredDepthMap;
    
    // Light matrices
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightSpaceMatrix;
    
    // Light properties
    glm::vec3 lightDirection;
    glm::vec3 lightPosition;
    glm::vec3 target;
    
    // Projection bounds
    float projLeft, projRight, projBottom, projTop, projNear, projFar;
    
    void init();
    void cleanup();
    void updateMatrices();
}; 
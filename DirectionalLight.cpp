#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 Dir, glm::vec3 ambientCol, glm::vec3 specular, glm::vec3 diffuse)
    : Light(glm::vec3(0.0f, 20.0f, 0.0f), ambientCol, specular, diffuse)
{
    std::cout << "Created Directional Light" << std::endl;
    this->Direction = Dir;
    this->ambient = ambientCol;
    this->specular = specular;
    this->diffuse = diffuse;
    lightModel.setName("Directional Light" + std::to_string(numberOfLights));    
}

void DirectionalLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setInt("lights[" + std::to_string(index) + "].state", this->state);
    shader.setVec3("lights[" + std::to_string(index) + "].direction", this->Direction);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setInt("numLights", numberOfLights);
}

void DirectionalLight::setDeferredUniforms(Shader& shader, int index, const glm::vec3*, const glm::vec3*) const {
    std::string prefix = "dirLights[" + std::to_string(index) + "]";
    shader.setVec3(prefix + ".direction", this->Direction);
    shader.setVec3(prefix + ".ambient", this->ambient);
    shader.setVec3(prefix + ".diffuse", this->diffuse);
    shader.setVec3(prefix + ".specular", this->specular);
}

int DirectionalLight::getType() const {
    return TYPE_DIRECTIONAL;
}
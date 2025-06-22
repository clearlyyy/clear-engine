#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(glm::vec3 Dir, glm::vec3 ambientCol, glm::vec3 specular, glm::vec3 diffuse)
    : Light(glm::vec3(0.0f, 20.0f, 0.0f), ambientCol, specular, diffuse)
{
    std::cout << "Created Directional Light" << std::endl;
    this->Direction = Dir;
    this->ambient = ambientCol;
    this->specular = specular;
    this->diffuse = diffuse;
}

void DirectionalLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setVec3("lights[" + std::to_string(index) + "].direction", this->Direction);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setInt("numLights", numberOfLights);
}

int DirectionalLight::getType() const {
    return TYPE_DIRECTIONAL;
}
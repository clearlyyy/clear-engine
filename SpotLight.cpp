#include "SpotLight.h"

SpotLight::SpotLight(float cutoff, float outerCutoff, glm::vec3 Position, glm::vec3 dir, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse)
    :  Light(Position, ambient, specular, diffuse), cutOff(cutoff), outerCutOff(outerCutoff)
{
    std::cout << "Created Spot Light" << std::endl;
    this->Position = Position;
    this->ambient = ambient;
    this->specular = specular;
    this->diffuse = diffuse;
    this->Direction = dir;
}

void SpotLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setVec3("lights[" + std::to_string(index) + "].position", this->Position);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setFloat("lights[" + std::to_string(index) + "].cutOff", this->cutOff);
    shader.setFloat("lights[" + std::to_string(index) + "].outerCutOff", this->outerCutOff);
    shader.setVec3("lights[" + std::to_string(index) + "].direction", this->Direction);

    shader.setInt("numLights", numberOfLights);
}

int SpotLight::getType() const {
    return TYPE_SPOT;
}

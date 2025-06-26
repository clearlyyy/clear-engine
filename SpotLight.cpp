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
    lightModel.setName("Spot Light " + std::to_string(numberOfLights));
    
}

void SpotLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setInt("lights[" + std::to_string(index) + "].state", this->state);
    shader.setVec3("lights[" + std::to_string(index) + "].position", this->Position);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setFloat("lights[" + std::to_string(index) + "].cutOff", this->cutOff);
    shader.setFloat("lights[" + std::to_string(index) + "].outerCutOff", this->outerCutOff);
    shader.setVec3("lights[" + std::to_string(index) + "].direction", this->Direction);

    shader.setInt("numLights", numberOfLights);
}

void SpotLight::setDeferredUniforms(Shader& shader, int index, const glm::vec3* viewSpacePos, const glm::vec3* viewSpaceDir) const {
    std::string prefix = "spotLights[" + std::to_string(index) + "]";
    if (viewSpacePos) {
        shader.setVec3(prefix + ".position", *viewSpacePos);
    } else {
        shader.setVec3(prefix + ".position", this->Position);
    }
    if (viewSpaceDir) {
        shader.setVec3(prefix + ".direction", *viewSpaceDir);
    } else {
        shader.setVec3(prefix + ".direction", this->Direction);
    }
    shader.setVec3(prefix + ".ambient", this->ambient);
    shader.setVec3(prefix + ".diffuse", this->diffuse);
    shader.setVec3(prefix + ".specular", this->specular);
    shader.setFloat(prefix + ".cutOff", this->cutOff);
    shader.setFloat(prefix + ".outerCutOff", this->outerCutOff);
    shader.setFloat(prefix + ".constant", 1.0f);
    shader.setFloat(prefix + ".linear", 0.09f);
    shader.setFloat(prefix + ".quadratic", 0.032f);
}

int SpotLight::getType() const {
    return TYPE_SPOT;
}

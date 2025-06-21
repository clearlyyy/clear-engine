#include "PointLight.h"

PointLight::PointLight(glm::vec3 Position, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse)
    : Light(Position, ambient, specular, diffuse)
{
    std::cout << "Created Point Light" << std::endl;
    this->Position = Position;
    this->ambient = ambient;
    this->specular = specular;
    this->diffuse = diffuse;
}

void PointLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setVec3("lights[" + std::to_string(index) + "].position", this->Position);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setInt("numLights", numberOfLights);
}

int PointLight::getType() const {
    return TYPE_POINT;
}


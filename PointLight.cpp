#include "PointLight.h"

PointLight::PointLight(glm::vec3 Position, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse)
    : Light(Position, ambient, specular, diffuse)
{
    std::cout << "Created Point Light" << std::endl;
    this->Position = Position;
    this->ambient = ambient;
    this->specular = specular;
    this->diffuse = diffuse;
    
    this->constant = 1.0f;
    this->linear = 0.09f;
    this->quadratic = 0.032f;

}

void PointLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setVec3("lights[" + std::to_string(index) + "].position", this->Position);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setFloat("lights[" + std::to_string(index) + "].constant", this->constant);
    shader.setFloat("lights[" + std::to_string(index) + "].linear", this->linear);
    shader.setFloat("lights[" + std::to_string(index) + "].quadratic", this->quadratic);

    shader.setInt("numLights", numberOfLights);
}

int PointLight::getType() const {
    return TYPE_POINT;
}

void PointLight::setFalloff(float constant, float linear, float quadratic)
{
    this->constant = constant;
    this->linear = linear;
    this->quadratic = quadratic;
}


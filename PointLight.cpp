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

    lightModel.setName("Point Light " + std::to_string(numberOfLights) );
}

void PointLight::applyToShader(Shader& shader, int index) const {
    shader.use();
    shader.setInt("lights[" + std::to_string(index) + "].type", getType());
    shader.setInt("lights[" + std::to_string(index) + "].state", this->state);
    shader.setVec3("lights[" + std::to_string(index) + "].position", this->Position);
    shader.setVec3("lights[" + std::to_string(index) + "].ambient", this->ambient);
    shader.setVec3("lights[" + std::to_string(index) + "].diffuse", this->diffuse);
    shader.setVec3("lights[" + std::to_string(index) + "].specular", this->specular);

    shader.setFloat("lights[" + std::to_string(index) + "].constant", this->constant);
    shader.setFloat("lights[" + std::to_string(index) + "].linear", this->linear);
    shader.setFloat("lights[" + std::to_string(index) + "].quadratic", this->quadratic);

    shader.setInt("numLights", numberOfLights);
}

void PointLight::setDeferredUniforms(Shader& shader, int index, const glm::vec3* viewSpacePos, const glm::vec3*) const {
    std::string prefix = "pointLights[" + std::to_string(index) + "]";
    if (viewSpacePos) {
        shader.setVec3(prefix + ".position", *viewSpacePos);
    } else {
        shader.setVec3(prefix + ".position", this->Position);
    }
    shader.setVec3(prefix + ".ambient", this->ambient);
    shader.setVec3(prefix + ".diffuse", this->diffuse);
    shader.setVec3(prefix + ".specular", this->specular);
    shader.setFloat(prefix + ".constant", this->constant);
    shader.setFloat(prefix + ".linear", this->linear);
    shader.setFloat(prefix + ".quadratic", this->quadratic);
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


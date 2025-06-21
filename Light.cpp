#include "Light.h"

int Light::numberOfLights = 0;

Light::Light(glm::vec3 Position, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse)
{
    /// Maybe do something here in the future
    // Time to do something with it
    Light::numberOfLights++;
    // Create a Light Shader, a Light Material and a Light Object.
    // This is for visualization purposes.
    material = Material(&lightShader);
    lightMesh = Cube(&material);
    lightModel = Model(Position, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.2f, 0.2f, 0.2f));
    lightModel.addMesh(lightMesh);
    lightModel.setPosition(Position);
}

void Light::setPosition(glm::vec3 newPos)
{
    Position = newPos;
}
void Light::setAmbient(glm::vec3 ambient)
{
    this->ambient = ambient;
}
void Light::setSpecular(glm::vec3 specular)
{
    this->specular = specular;
}
void Light::setDiffuse(glm::vec3 diffuse)
{
    this->diffuse = diffuse;
}

Model& Light::getLightModel() {
    return lightModel;
}


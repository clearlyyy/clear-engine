#include "Material.h"

Material::Material() : shader(nullptr), diffuseMap(nullptr), specularMap(nullptr), shininess(0.0f) {}

Material::Material(Shader* shaderProgram)
    : shader(shaderProgram), diffuseMap(nullptr), specularMap(nullptr), shininess(0.0f)
{}

void Material::Apply()
{
    shader->use();
    if (diffuseMap) {
        diffuseMap->use();
        diffuseMap->setTexture(*shader, "material.diffuse");
    }
    if (specularMap) {
        specularMap->use();
        specularMap->setTexture(*shader, "material.specular");
    }
    shader->setFloat("material.shininess", shininess);
}
#include "Material.h"

Material::Material() : shader(nullptr), diffuseColor(glm::vec3(1.0f)), shininess(32.0f), diffuseMap(nullptr), specularMap(nullptr), materialIndex(0), hasTransparency(false) {}

Material::Material(Shader* shaderProgram)
    : shader(shaderProgram), diffuseColor(glm::vec3(1.0f)), shininess(32.0f), diffuseMap(nullptr), specularMap(nullptr), materialIndex(0), hasTransparency(false)
{}

void Material::Apply()
{
    Apply(materialIndex);
}

void Material::Apply(int index)
{
    if (!shader) {
        std::cout << "Warning: Material has no shader assigned!" << std::endl;
        return;
    }
    
    shader->use();
    
    // Set the current material index
    shader->setInt("currentMaterialIndex", index);
    
    if (diffuseMap) {
        diffuseMap->use();
        diffuseMap->setTexture(*shader, "materialDiffuse[" + std::to_string(index) + "]");
    }
    if (specularMap) {
        specularMap->use();
        specularMap->setTexture(*shader, "materialSpecular[" + std::to_string(index) + "]");
    }
    shader->setFloat("materials[" + std::to_string(index) + "].shininess", shininess);
}
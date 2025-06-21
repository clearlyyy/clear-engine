#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Texture.h"


// Material Class.
class Material {
    public:
        Material();
        Material(Shader* shader);
        Shader* shader;
        glm::vec3 diffuseColor;
        float shininess;
        Texture* diffuseMap;
        Texture* specularMap;

    void Apply();
};
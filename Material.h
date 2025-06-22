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
        int materialIndex; // Index in the shader's material array
        bool hasTransparency; // Flag to indicate if this material has transparency

    void Apply();
    void Apply(int index); // Apply to specific material index
};
#pragma once
#include <glm/glm.hpp>
#include "Shader.h"
#include "Primitives.h"
#include "Model.h"

enum LightType {
    TYPE_POINT,
    TYPE_DIRECTIONAL,
    TYPE_SPOT
};

class Light {
    public:
        static int numberOfLights;
        glm::vec3 Position = glm::vec3(0);
        glm::vec3 ambient;
        glm::vec3 specular;
        glm::vec3 diffuse;
    public:
        Light(glm::vec3 Position, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);

        virtual void applyToShader(Shader& shader, int index) const = 0;
        virtual int getType() const = 0;

        void setPosition(glm::vec3 newPos);
        void setAmbient(glm::vec3 ambient);
        void setSpecular(glm::vec3 specular);
        void setDiffuse(glm::vec3 diffuse);

        Model& getLightModel();

    private:
        // Light Model for visualization purposes.
        Shader lightShader = Shader("lightcube.vs", "lightcube.fs");
        Material material;
        Cube lightMesh;
        Model lightModel;
};
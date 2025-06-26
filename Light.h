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
        virtual void setDeferredUniforms(Shader& shader, int index, const glm::vec3* viewSpacePos = nullptr, const glm::vec3* viewSpaceDir = nullptr) const = 0;

        void setPosition(glm::vec3 newPos);
        void setAmbient(glm::vec3 ambient);
        void setSpecular(glm::vec3 specular);
        void setDiffuse(glm::vec3 diffuse);

        void setState(bool state, Shader& shader, int index);
        
        Model& getLightModel();
        
        std::string name;
        bool state = true;
        bool enabled = true;
        virtual void setEnabled(bool state) { enabled = state; }
        virtual bool isEnabled() const { return enabled; }
    private:
        // Light Model for visualization purposes.
        Shader lightShader = Shader("lightcube.vs", "lightcube.fs");
        Material material;
        Cube lightMesh;
    protected:
        Model lightModel;
};
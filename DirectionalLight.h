#include "Light.h"

class DirectionalLight : public Light {
    public:
    DirectionalLight(glm::vec3 Direction, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);
    void applyToShader(Shader& shader, int index) const override;
    int getType() const override;
    void setDeferredUniforms(Shader& shader, int index, const glm::vec3* viewSpacePos = nullptr, const glm::vec3* viewSpaceDir = nullptr) const override;
    glm::vec3 Direction;
};
#include "Light.h"

class DirectionalLight : public Light {
    public:
    DirectionalLight(glm::vec3 Direction, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);
    void applyToShader(Shader& shader, int index) const override;
    int getType() const override;
    glm::vec3 Direction;
};
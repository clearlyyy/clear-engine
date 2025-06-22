#include "Light.h"


class SpotLight : public Light
{
    public:
    SpotLight(float cutoff, float outerCutoff, glm::vec3 Position, glm::vec3 Direction, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);

    void applyToShader(Shader& shader, int index) const override;
    int getType() const override;
    
    float cutOff;
    float outerCutOff;
    glm::vec3 Direction;
};
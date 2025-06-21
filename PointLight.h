
#include "Light.h"


class PointLight : public Light 
{   
    public:

    PointLight(glm::vec3 Position, glm::vec3 ambient, glm::vec3 specular, glm::vec3 diffuse);
    
    void applyToShader(Shader& shader, int index) const override;
    int getType() const override;
};
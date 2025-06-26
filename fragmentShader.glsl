#version 440 core
out vec4 FragColor;

#define MAX_LIGHTS 8
#define MAX_MATERIALS 32

// Declare samplers at global scope (OpenGL requirement)
uniform sampler2D materialDiffuse[MAX_MATERIALS];
uniform sampler2D materialSpecular[MAX_MATERIALS];

struct Material {
    float shininess;
};

struct Light {
    int type;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 direction;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;

    int state; // 0 = off, 1 = on
};

in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightColor;
uniform vec3 viewPos;

uniform Material materials[MAX_MATERIALS];
uniform int currentMaterialIndex;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0); 

    // Get the current material
    Material material = materials[currentMaterialIndex];

    for (int i = 0; i < numLights; i++) {

        if (lights[i].state == 1) {
            // Ambient
            vec3 ambient = lights[i].ambient * texture(materialDiffuse[currentMaterialIndex], TexCoord).rgb;

            // Diffuse
            vec3 lightDir;

            float attenuation = 1;
            float intensity = 1;

            if (lights[i].type == 0) {
                lightDir = normalize(lights[i].position - FragPos);
                float distance    = length(lights[i].position - FragPos);
                attenuation = 1.0 / (lights[i].constant + lights[i].linear * distance + 
    	    	    lights[i].quadratic * (distance * distance));  
            } else if (lights[i].type == 1) {
                lightDir = normalize(-lights[i].direction);
            } else if (lights[i].type == 2) {
                lightDir = normalize(lights[i].position - FragPos);
                float theta = dot(lightDir, normalize(-lights[i].direction));
                float epsilon = (lights[i].cutOff - lights[i].outerCutOff);
                intensity = clamp((theta - lights[i].outerCutOff) / epsilon, 0.0, 1.0);
                attenuation = 1.0;
            }
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = lights[i].diffuse * diff * texture(materialDiffuse[currentMaterialIndex], TexCoord).rgb;

            //Specular
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
            vec3 specular = lights[i].specular * spec * vec3(texture(materialSpecular[currentMaterialIndex], TexCoord));

            ambient *= attenuation;
            diffuse *= attenuation * intensity;
            specular *= attenuation * intensity;

            result += ambient + diffuse + specular;
        }
    }

    // Get alpha from diffuse texture for transparency
    float alpha = texture(materialDiffuse[currentMaterialIndex], TexCoord).a;
    
    FragColor = vec4(result, alpha);
}
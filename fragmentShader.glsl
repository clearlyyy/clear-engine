#version 440 core
out vec4 FragColor;

#define MAX_LIGHTS 8

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    int type;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;


uniform vec3 lightColor;
uniform vec3 viewPos;

uniform Material material;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0); 

    for (int i = 0; i < numLights; i++) {
        // Ambient
        vec3 ambient = lights[i].ambient * texture(material.diffuse, TexCoord).rgb;

        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = lights[i].diffuse * diff * texture(material.diffuse, TexCoord).rgb;

        //Specular
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lights[i].specular * spec * vec3(texture(material.specular, TexCoord));
        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
}
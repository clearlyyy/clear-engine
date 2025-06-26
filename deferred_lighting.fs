#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;
uniform sampler2D gPositionWorld;
uniform sampler2D gSpecular;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;

#define MAX_POINT_LIGHTS 8
#define MAX_DIR_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float constant;
    float linear;
    float quadratic;
};

uniform int numPointLights;
uniform int numDirLights;
uniform int numSpotLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform DirLight dirLights[MAX_DIR_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform vec3 viewPos;

float ShadowCalculation(vec3 fragPosWorld, vec3 normal, vec3 lightDir)
{
    // Reconstruct light-space position
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPosWorld, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Early out if outside shadow map
    if(projCoords.z > 1.0) return 0.0;

    float shadow = 0.0;
    float currentDepth = projCoords.z;
    // Slope-scale bias
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);

    // 3x3 PCF with clamped coordinates
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            vec2 sampleCoords = clamp(projCoords.xy + offset, 0.0, 1.0);
            float pcfDepth = texture(shadowMap, sampleCoords).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    return shadow;
}

void main()
{
    // Retrieve data from G-buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 FragPosWorld = texture(gPositionWorld, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedo, TexCoords).rgb;
    float alpha = texture(gAlbedo, TexCoords).a;
    float shininess = clamp(alpha * 128.0, 1.0, 128.0);
    vec3 SpecularMap = texture(gSpecular, TexCoords).rgb;
    float ssaoFactor = clamp(texture(ssao, TexCoords).r, 0.2, 1.0);

    if (length(FragPos) < 0.0001) {
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        return;
    }

    vec3 result = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Point lights
    for (int i = 0; i < numPointLights; ++i) {
        PointLight light = pointLights[i];
        // Ambient
        vec3 ambient = light.ambient * Albedo * ssaoFactor;
        // Diffuse
        vec3 lightDir = normalize(light.position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * Albedo * mix(1.0, ssaoFactor, 0.5);
        // Specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light.specular * spec * SpecularMap;
        // Attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        result += ambient + diffuse + specular;
    }

    // Directional lights
    for (int i = 0; i < numDirLights; ++i) {
        DirLight light = dirLights[i];
        // Ambient
        vec3 ambient = light.ambient * Albedo * ssaoFactor;
        // Diffuse
        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * Albedo * mix(1.0, ssaoFactor, 0.5);
        // Specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light.specular * spec * SpecularMap;

        //Shadow
        float shadow = ShadowCalculation(FragPosWorld, Normal, lightDir);
        result += ambient + (1.0 - shadow) * (diffuse + specular);
    }

    // Spot lights
    for (int i = 0; i < numSpotLights; ++i) {
        SpotLight light = spotLights[i];
        // Ambient
        vec3 ambient = light.ambient * Albedo * ssaoFactor;
        // Diffuse
        vec3 lightDir = normalize(light.position - FragPos);
        float theta = dot(lightDir, normalize(-light.direction));
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * Albedo * mix(1.0, ssaoFactor, 0.5);
        // Specular
        vec3 reflectDir = reflect(-lightDir, Normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light.specular * spec * SpecularMap;
        // Attenuation
        float distance = length(light.position - FragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
        ambient *= attenuation * intensity;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;
        result += ambient + diffuse + specular;
    }

    FragColor = vec4(result, 1.0);
} 
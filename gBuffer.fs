#version 330 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gPositionWorld;
layout (location = 2) out vec4 gNormal;
layout (location = 3) out vec4 gAlbedo;
layout (location = 4) out vec4 gSpecular;

in vec3 FragPos;
in vec3 FragPosWorld;
in vec3 Normal;
in vec2 TexCoords;

// Material system uniforms
uniform sampler2D materialDiffuse[32]; // Array of diffuse textures
uniform sampler2D materialSpecular[32]; // Array of specular textures
uniform int currentMaterialIndex;
uniform float materialShininess[32]; // Array of shininess values per material

void main()
{
    // Store view-space position
    gPosition = vec4(FragPos, 1.0);
    //Store world-space position
    gPositionWorld = vec4(FragPosWorld, 1.0);
    // Store view-space normal
    gNormal = vec4(normalize(Normal), 1.0);
    // Store albedo - use material diffuse texture if available, otherwise use default color
    vec4 albedo;
    if (currentMaterialIndex >= 0 && currentMaterialIndex < 32) {
        albedo = texture(materialDiffuse[currentMaterialIndex], TexCoords);
        // Store shininess in alpha channel, normalized
        albedo.a = clamp(materialShininess[currentMaterialIndex] / 128.0, 0.0, 1.0);
    } else {
        // Default white color if no material is set
        albedo = vec4(1.0, 1.0, 1.0, 32.0 / 128.0); // Default shininess 32
    }
    gAlbedo = albedo;

    vec4 specular = vec4(1.0);
    if (currentMaterialIndex >= 0 && currentMaterialIndex < 32) {
        specular = texture(materialSpecular[currentMaterialIndex], TexCoords);
    }
    gSpecular = vec4(specular.rgb, 1.0);
}
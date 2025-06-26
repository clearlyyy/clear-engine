#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D shadowMap;
uniform vec2 texelSize;
uniform int blurRadius;

void main()
{
    float result = 0.0;
    float totalWeight = 0.0;
    
    // Gaussian weights for blur
    float weights[9] = float[9](
        0.0625, 0.125, 0.0625,
        0.125,  0.25,  0.125,
        0.0625, 0.125, 0.0625
    );
    
    // Apply Gaussian blur
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(x, y) * texelSize;
            vec2 sampleCoords = clamp(TexCoords + offset, 0.0, 1.0);
            float depth = texture(shadowMap, sampleCoords).r;
            
            float weight = weights[(x + 1) * 3 + (y + 1)];
            result += depth * weight;
            totalWeight += weight;
        }
    }
    
    FragColor = result / totalWeight;
} 
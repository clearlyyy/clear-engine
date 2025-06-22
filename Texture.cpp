#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static counter to track texture units
static int textureUnitCounter = 0;

Texture::Texture(const char* path, bool transparent)
{
    std::cout << "Creating texture from: " << path << std::endl;
    
    //Create Texture and Bind it
    glGenTextures(1, &ID);
    
    // Check if texture ID is valid
    if (ID == 0) {
        std::cout << "Failed to generate texture ID for: " << path << std::endl;
        return;
    }
    
    glBindTexture(GL_TEXTURE_2D, ID);
    
    // Set texture wrapping/filter options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    
    // Load the image
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        std::cout << "Image loaded successfully: " << width << "x" << height << " channels: " << nrChannels << std::endl;
        
        // Check if texture size is reasonable and won't cause crashes
        int maxSize = 8192; // Allow up to 8192x8192 textures
        if (width <= 0 || height <= 0 || width > maxSize || height > maxSize) {
            std::cout << "Texture too large or invalid size: " << width << "x" << height << " (max: " << maxSize << "x" << maxSize << ")" << std::endl;
            stbi_image_free(data);
            ID = 0; // Mark as failed
            return;
        }
        
        // Check OpenGL errors before uploading
        GLenum error = glGetError();
        
        try {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            
            // Check for OpenGL errors after upload
            error = glGetError();
            if (error != GL_NO_ERROR) {
                std::cout << "OpenGL error after texture upload: " << error << std::endl;
                ID = 0; // Mark as failed
                stbi_image_free(data);
                return;
            }
            
            glGenerateMipmap(GL_TEXTURE_2D);
            
            // Check for OpenGL errors after mipmap generation
            error = glGetError();
            
        } catch (...) {
            std::cout << "Exception during texture upload" << std::endl;
            ID = 0; // Mark as failed
            stbi_image_free(data);
            return;
        }
        
        // Free the image from memory only if data was successfully loaded
        stbi_image_free(data);
        std::cout << "Texture created successfully: " << path << " (ID: " << ID << ")" << std::endl;
    } else {
        std::cout << "Failed to load image data: " << path << " - " << stbi_failure_reason() << std::endl;
        ID = 0; // Mark as failed
    }

    // Assign texture unit and create uniform name
    textureUnit = textureUnitCounter++;
    textureUniformID = "texture" + std::to_string(textureUnit);
}

void Texture::use()
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

// We assume currently that the shader is using the texturex naming convention
void Texture::setTexture(Shader shaderProgram, const std::string& type) {
    shaderProgram.setInt(type, textureUnit);
}

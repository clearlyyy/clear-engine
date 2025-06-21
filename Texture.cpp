#include "Texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Static counter to track texture units
static int textureUnitCounter = 0;

Texture::Texture(const char* path, bool transparent)
{
    //Create Texture and Bind it
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    // Set texture wrapping/filter options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        if (transparent)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "FAILED TO LOAD TEXTURE: " << path << std::endl;
    }
    // Free the image from memory
    stbi_image_free(data);

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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include "Shader.h"

class Texture {
    private:
    int width, height, nrChannels;
    std::string textureUniformID;
    int textureUnit;

    public:
    unsigned int ID;
    Texture(const char* path, bool transparent);

    void use();
    void setTexture(Shader shaderProgram, const std::string& type);
};

#endif // TEXTURE_H
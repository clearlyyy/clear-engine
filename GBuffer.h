#pragma once
#include <glad/glad.h>
#include <vector>

class GBuffer {
public:
    GBuffer(unsigned int width, unsigned int height);
    ~GBuffer();

    void bindForWriting();
    void bindForReading();
    void unbind();

    GLuint getPositionTexture() const { return gPosition; }
    GLuint getNormalTexture() const { return gNormal; }
    GLuint getAlbedoTexture() const { return gAlbedo; }
    GLuint getFBO() const { return gBufferFBO; }
    GLuint getWorldPositionTexture() const { return gPositionWorld; }
    GLuint getSpecularTexture() const { return gSpecular; }
    void resize(unsigned int width, unsigned int height);

private:
    GLuint gBufferFBO;
    GLuint gPosition, gNormal, gAlbedo, gPositionWorld, gSpecular;
    GLuint rboDepth;
    unsigned int width, height;
    void init();
    void cleanup();
}; 
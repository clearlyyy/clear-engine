#include "GBuffer.h"
#include <iostream>

GBuffer::GBuffer(unsigned int width, unsigned int height)
    : width(width), height(height), gBufferFBO(0), gPosition(0), gNormal(0), gAlbedo(0), gSpecular(0), rboDepth(0)
{
    init();
}

GBuffer::~GBuffer() {
    cleanup();
}

void GBuffer::init() {
    // Create FBO
    glGenFramebuffers(1, &gBufferFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);

    // Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // World Position color buffer
    glGenTextures(1, &gPositionWorld);
    glBindTexture(GL_TEXTURE_2D, gPositionWorld);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gPositionWorld, 0);

    // Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

    // Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedo, 0);

    // Specular color buffer
    glGenTextures(1, &gSpecular);
    glBindTexture(GL_TEXTURE_2D, gSpecular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gSpecular, 0);

    // Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, attachments);

    // Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::cleanup() {
    if (gPosition) glDeleteTextures(1, &gPosition);
    if (gNormal) glDeleteTextures(1, &gNormal);
    if (gAlbedo) glDeleteTextures(1, &gAlbedo);
    if (gSpecular) glDeleteTextures(1, &gSpecular);
    if (rboDepth) glDeleteRenderbuffers(1, &rboDepth);
    if (gBufferFBO) glDeleteFramebuffers(1, &gBufferFBO);
}

void GBuffer::bindForWriting() {
    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
}

void GBuffer::bindForReading() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBufferFBO);
}

void GBuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GBuffer::resize(unsigned int newWidth, unsigned int newHeight) {
    if (width == newWidth && height == newHeight) return;
    cleanup();
    width = newWidth;
    height = newHeight;
    init();
} 
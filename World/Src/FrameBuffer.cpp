#include "../Includes/FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(int w, int h) : width(w), height(h) {
    create();
}

FrameBuffer::~FrameBuffer() {
    destroy();
}

void FrameBuffer::create() {
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    glGenRenderbuffers(1, &depthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "FrameBuffer is not complete!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::destroy() {
    if (colorTexture) glDeleteTextures(1, &colorTexture);
    if (depthRBO) glDeleteRenderbuffers(1, &depthRBO);
    if (FBO) glDeleteFramebuffers(1, &FBO);
    colorTexture = depthRBO = FBO = 0;
}

void FrameBuffer::Resize(int newWidth, int newHeight) {
    if (newWidth == width && newHeight == height) return;
    destroy();
    width = newWidth;
    height = newHeight;
    create();
}

void FrameBuffer::Bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, width, height);
}

void FrameBuffer::Unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

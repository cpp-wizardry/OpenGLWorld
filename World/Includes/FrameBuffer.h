#pragma once
#include <glad/glad.h>

class FrameBuffer {
public:
    FrameBuffer(int width, int height);
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    void Bind() const;
    static void Unbind();

    void Resize(int newWidth, int newHeight);

    GLuint GetColorTexture() const { return colorTexture; }
    int Width() const { return width; }
    int Height() const { return height; }

private:
    void create();
    void destroy();

    GLuint FBO = 0, colorTexture = 0, depthRBO = 0;
    int width, height;
};

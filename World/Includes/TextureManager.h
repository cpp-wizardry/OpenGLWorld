#pragma once
#include <glad/glad.h>
#include <string>
#include <unordered_map>

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) noexcept = default;
    TextureManager& operator=(TextureManager&&) noexcept = default;

    GLuint LoadTexture(const std::string& path, bool flipVertically = true,
        GLenum wrapMode = GL_REPEAT, bool generateMipmaps = true);

    void ReleaseAll();

    void Release(const std::string& path);

    bool IsLoaded(const std::string& path) const { return cache.find(path) != cache.end(); }
    GLuint Get(const std::string& path) const;

private:
    std::unordered_map<std::string, GLuint> cache;
};
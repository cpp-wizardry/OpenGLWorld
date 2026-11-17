#include "../Includes/TextureManager.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../Src/ModelLoader/stb_image.h"

TextureManager::~TextureManager() {
    ReleaseAll();
}

GLuint TextureManager::LoadTexture(const std::string& path, bool flipVertically, GLenum wrapMode, bool generateMipmaps) {
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;

    stbi_set_flip_vertically_on_load(flipVertically);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "TextureManager: failed to load texture \"" << path << "\": " << stbi_failure_reason() << "\n";
        return 0;
    }

    GLenum format = GL_RGB;
    if (channels == 1) format = GL_RED;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 4) format = GL_RGBA;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    cache[path] = textureID;
    return textureID;
}

void TextureManager::ReleaseAll() {
    for (auto& pair : cache) {
        glDeleteTextures(1, &pair.second);
    }
    cache.clear();
}

void TextureManager::Release(const std::string& path) {
    auto it = cache.find(path);
    if (it == cache.end()) return;
    glDeleteTextures(1, &it->second);
    cache.erase(it);
}

GLuint TextureManager::Get(const std::string& path) const {
    auto it = cache.find(path);
    return (it != cache.end()) ? it->second : 0;
}
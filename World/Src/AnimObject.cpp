#include "../Includes/AnimObject.h"

AnimatedObject::AnimatedObject(const std::vector<GLfloat>& vertices, const std::vector<VertexAttribute>& layout, GLenum usage)
    : mesh(vertices, layout, usage) {
}

AnimatedObject::AnimatedObject(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices,
    const std::vector<VertexAttribute>& layout, GLenum usage)
    : mesh(vertices, indices, layout, usage) {
}
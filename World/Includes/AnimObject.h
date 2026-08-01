//AnimatedObject.h
#pragma once
#include "Animator.h"
#include "Mesh.h"


class AnimatedObject : public Animator
{
public:
    AnimatedObject() = default;

    AnimatedObject(const std::vector<GLfloat>& vertices, const std::vector<VertexAttribute>& layout,
        GLenum usage = GL_STATIC_DRAW);

    AnimatedObject(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices,
        const std::vector<VertexAttribute>& layout, GLenum usage = GL_STATIC_DRAW);

    AnimatedObject(const AnimatedObject&) = delete;
    AnimatedObject& operator=(const AnimatedObject&) = delete;
    AnimatedObject(AnimatedObject&&) noexcept = default;
    AnimatedObject& operator=(AnimatedObject&&) noexcept = default;

    void Draw(GLenum primitive = GL_TRIANGLES, bool useIndices = true) const { mesh.Draw(primitive, useIndices); }

    glm::mat4 GetModelMatrix() const { return GetPose().GetModelMatrix(); }

    Mesh& GetMesh() { return mesh; }
    const Mesh& GetMesh() const { return mesh; }

private:
    Mesh mesh;
};
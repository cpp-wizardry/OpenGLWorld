#pragma once
#include <glad/glad.h>
#include <vector>



struct VertexAttribute {
	GLuint location;
	GLint components;
};


class Mesh
{
public:
	Mesh() = default;

	Mesh(const std::vector<GLfloat>& vertices, const std::vector<VertexAttribute>& layout, GLenum usage = GL_STATIC_DRAW);

	Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, const std::vector<VertexAttribute>& layout, GLenum usage = GL_STATIC_DRAW);

	~Mesh();

	//operators
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;
	Mesh(Mesh&& other) noexcept;
	Mesh& operator=(Mesh&& other) noexcept;

	void UpdateVertices(const std::vector<GLfloat>& vertices);
	void UpdateIndices(const std::vector<GLuint>& indices);

	void Draw(GLenum primitive = GL_TRIANGLES, bool useIndices = true) const;

	bool IsValid() const { return VAO != 0; }

	size_t VertexCount() const { return vertexCount; }
	size_t IndexCount() const { return indexCount; }

private:
	void init(const std::vector<GLfloat>& vertices, const GLuint* indices, size_t indexCountIn, const std::vector<VertexAttribute>& layout, GLenum usageIn);
	void release();

	GLuint VAO = 0, VBO = 0, EBO = 0;
	GLsizei stride = 0;
	size_t vertexCount = 0;
	size_t indexCount = 0;
	bool hasIndices = false;
	GLenum usage = GL_STATIC_DRAW;

};
#include "../../Includes/Mesh.h"

Mesh::Mesh(const std::vector<GLfloat>& vertices, const std::vector<VertexAttribute>& layout, GLenum usageIn)
{
	init(vertices, nullptr, 0, layout, usageIn);
}

Mesh::Mesh(const std::vector<GLfloat>& vertices, const std::vector<GLuint>& indices, const std::vector<VertexAttribute>& layout, GLenum usageIn)
{
	init(vertices, indices.data(), indices.size(), layout, usageIn);
}

void Mesh::init(const std::vector<GLfloat>& vertices, const GLuint* indices, size_t indexCountIn, const std::vector<VertexAttribute>& layout, GLenum usageIn)
{
	usage = usageIn;
	hasIndices = indices != nullptr && indexCountIn > 0;

	GLint floatsPerVertex = 0;
    for (const auto& attr : layout) floatsPerVertex += attr.components;
    stride = static_cast<GLsizei>(floatsPerVertex * sizeof(float));

    vertexCount = floatsPerVertex > 0 ? vertices.size() / floatsPerVertex : 0;
    indexCount = indexCountIn;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!vertices.empty()) {
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), usage);
    }

    size_t offsetFloats = 0;
    for (const auto& attr : layout) {
        glVertexAttribPointer(attr.location, attr.components, GL_FLOAT, GL_FALSE,
            stride, (void*)(offsetFloats * sizeof(float)));
        glEnableVertexAttribArray(attr.location);
        offsetFloats += attr.components;
    }

    if (hasIndices) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCountIn * sizeof(unsigned int), indices, usage);
    }

    glBindVertexArray(0);
}

Mesh::~Mesh() { release(); }

void Mesh::release() {
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    VAO = VBO = EBO = 0;
}

Mesh::Mesh(Mesh&& other) noexcept {
    *this = std::move(other);
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        release();
        VAO = other.VAO; VBO = other.VBO; EBO = other.EBO;
        stride = other.stride;
        vertexCount = other.vertexCount;
        indexCount = other.indexCount;
        hasIndices = other.hasIndices;
        usage = other.usage;
        other.VAO = other.VBO = other.EBO = 0;
        other.vertexCount = other.indexCount = 0;
    }
    return *this;
}

void Mesh::UpdateVertices(const std::vector<float>& vertices) {
    if (!VAO) return;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), usage);

    if (stride > 0) vertexCount = vertices.size() / (stride / sizeof(float));
}

void Mesh::UpdateIndices(const std::vector<GLuint>& indices) {
    if (!VAO) return;
    if (!EBO) {
        glBindVertexArray(VAO);
        glGenBuffers(1, &EBO);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);
    indexCount = indices.size();
    hasIndices = indexCount > 0;
}

void Mesh::Draw(GLenum primitive, bool useIndices) const {
    if (!VAO) return;
    glBindVertexArray(VAO);
    if (useIndices && hasIndices && indexCount > 0) {
        glDrawElements(primitive, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, 0);
    }
    else if (vertexCount > 0) {
        glDrawArrays(primitive, 0, static_cast<GLsizei>(vertexCount));
    }
    glBindVertexArray(0);
}


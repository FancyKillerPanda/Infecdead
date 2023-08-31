#include "graphics/vertex_array.hpp"

#include <glad/glad.h>

u32 VertexArray::from_data(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
    // Vertex array.
    u32 vao = 0;
    glCreateVertexArrays(1, &vao);

    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));

    // Vertex buffer.
    u32 vbo = 0;
    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    // Index buffer.
    u32 ibo = 0;
    glCreateBuffers(1, &ibo);
    glNamedBufferData(ibo, indices.size() * sizeof(u32), indices.data(), GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ibo);

    return vao;
}

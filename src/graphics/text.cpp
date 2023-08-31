#include "graphics/text.hpp"
#include "graphics/vertex.hpp"
#include "graphics/vertex_array.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <string.h>
#include <vector>

Text::Text(const BitmapFont* font, const u8* string) : font(font) {
    if (shader.get_id() == 0) {
        // Not yet initialised.
        shader = { "res/shaders/text.vert", "res/shaders/text.frag" };
        glProgramUniform1i(shader.get_id(), glGetUniformLocation(shader.get_id(), "texSampler"), 0);
    }

    u32 stringLength = strlen(string);

    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    vertices.reserve(stringLength * 4);
    indices.reserve(stringLength * 6);

    for (u32 i = 0; i < stringLength; i++) {
    }

    vertices.emplace_back(Vertex { glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f) });
    vertices.emplace_back(Vertex { glm::vec2(0.0f, 512.0f), glm::vec2(0.0f, 0.0f) });
    vertices.emplace_back(Vertex { glm::vec2(512.0f, 512.0f), glm::vec2(1.0f, 0.0f) });
    vertices.emplace_back(Vertex { glm::vec2(512.0f, 0.0f), glm::vec2(1.0f, 1.0f) });

    indices.emplace_back(0);
    indices.emplace_back(1);
    indices.emplace_back(2);
    indices.emplace_back(2);
    indices.emplace_back(3);
    indices.emplace_back(0);

    vao = VertexArray::from_data(vertices, indices);

    isInitialised = true;
}

void Text::render(glm::vec2 position) {
    if (!isInitialised) {
        log_::warn("Trying to render uninitialised text.");
        return;
    }

    glBindVertexArray(vao);
    glUseProgram(shader.get_id());
    glBindTextureUnit(0, font->get_texture().get_id());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

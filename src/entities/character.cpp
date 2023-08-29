#include "entities/character.hpp"
#include "game/game.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

Character::Character(glm::vec2 position) : position(position) {
    init_common();
}

void Character::init_common() {
    if (isCommonInitialised) {
        return;
    }

    dimensions = glm::vec2 { 32.0f, 32.0f };

    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glEnableVertexArrayAttrib(vao, 1);
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord));

    Vertex vertices[] = {
        //  position
        { { -dimensions.x / 2, -dimensions.y / 2 }, { 0.0f, 1.0f } },
        { { -dimensions.x / 2, dimensions.y / 2 }, { 0.0f, 0.0f } },
        { { dimensions.x / 2, dimensions.y / 2 }, { 1.0f, 0.0f } },
        { { dimensions.x / 2, -dimensions.y / 2 }, { 1.0f, 1.0f } },
    };

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    u32 indices[] = { 0, 1, 2, 2, 3, 0 };
    glCreateBuffers(1, &ibo);
    glNamedBufferData(ibo, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ibo);

    shader = Game::get().get_character_shader().get_program_id();
    modelMatrixUniformLocation = glGetUniformLocation(shader, "model");
    columnUniformLocation = glGetUniformLocation(shader, "spritesheetColumn");
    rotationUniformLocation = glGetUniformLocation(shader, "rotation");
    glProgramUniform1i(shader, glGetUniformLocation(shader, "texSampler"), 0);

    log_::info("Initialised static Character data.");
    isCommonInitialised = true;
}

void Character::render() {
    if (!isCommonInitialised) {
        log_::warn("Trying to render Player without initialisation.");
        return;
    }

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
    glProgramUniformMatrix4fv(shader, modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glProgramUniform1i(shader, columnUniformLocation, spritesheetColumn);
    glProgramUniform1f(shader, rotationUniformLocation, rotation);

    glBindVertexArray(vao);
    glUseProgram(shader);
    glBindTextureUnit(0, get_current_texture().get_id());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

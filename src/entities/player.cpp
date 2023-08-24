#include "entities/player.hpp"
#include "game/game.hpp"
#include "graphics/vertex.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(glm::vec2 position) : position(position) {
}

void Player::init(Shader* shaderProgram) {
    shader = shaderProgram;
    dimensions = glm::vec2 { 128.0, 256.0 };

    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));

    // TODO(fkp): Different vertex structs for different types. We're unnecessarily
    // sending an empty texture coordinate vec2 here.
    Vertex vertices[] = {
        //  position
        { { 0.0, 0.0 } },
        { { 0.0, dimensions.y } },
        { { dimensions.x, dimensions.y } },
        { { dimensions.x, 0.0 } },
    };

    glCreateBuffers(1, &vbo);
    glNamedBufferData(vbo, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
    glVertexArrayAttribBinding(vao, 0, 0);

    u32 indices[] = { 0, 1, 2, 2, 3, 0 };
    glCreateBuffers(1, &ibo);
    glNamedBufferData(ibo, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexArrayElementBuffer(vao, ibo);

    modelMatrixUniformLocation = glGetUniformLocation(shader->get_program_id(), "model");

    // Textures
    if (spritesheetTextures.size() > 0) {
        // Already initialised, no need to do anything.
        return;
    }

    // NOTE(fkp): The order of items here must be the same as the order of enum variants in get_current_texture().
    spritesheetTextures.emplace_back("res/textures/characters/player/walk.png");

    initialised = true;
}

void Player::render() {
    if (!initialised) {
        log_::warn("Trying to render Player without initialisation.");
        return;
    }

    glBindVertexArray(vao);

    shader->use();
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(position.x, position.y, 0.0));
    glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindTextureUnit(0, get_current_texture().get_id());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

const Texture& Player::get_current_texture() {
    if (spritesheetTextures.size() == 0) {
        log_::error("Attempting to get player texture before initialisation.");
        return Texture();
    }

    // NOTE(fkp): The order of items here must be the same as the order of items in init_textures().
    enum SpritesheetState {
        WALK,
    };

    return spritesheetTextures[WALK];
}

#include "entities/character.hpp"
#include "game/game.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex.hpp"
#include "graphics/vertex_array.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

constexpr const f32 SPRITESHEET_NEXT_FRAME_DELTA = 0.15f;

Character::Character(glm::vec2 position) : position(position) {
    init_common();
}

void Character::init_common() {
    if (isCommonInitialised) {
        return;
    }

    dimensions = glm::vec2 { 32.0f, 32.0f };

    std::vector<Vertex> vertices = {
        //  position
        { { -dimensions.x / 2, -dimensions.y / 2 }, { 0.0f, 1.0f } },
        { { -dimensions.x / 2, dimensions.y / 2 }, { 0.0f, 0.0f } },
        { { dimensions.x / 2, dimensions.y / 2 }, { 1.0f, 0.0f } },
        { { dimensions.x / 2, -dimensions.y / 2 }, { 1.0f, 1.0f } },
    };

    std::vector<u32> indices = { 0, 1, 2, 2, 3, 0 };
    vao = VertexArray::from_data(vertices, indices);

    shader = Game::get().get_character_shader().get_program_id();
    modelMatrixUniformLocation = glGetUniformLocation(shader, "model");
    columnUniformLocation = glGetUniformLocation(shader, "spritesheetColumn");
    rotationUniformLocation = glGetUniformLocation(shader, "rotation");
    glProgramUniform1i(shader, glGetUniformLocation(shader, "texSampler"), 0);

    log_::info("Initialised static Character data.");
    isCommonInitialised = true;
}

void Character::update_position(f32 deltaTime) {
    velocity += acceleration * deltaTime;
    velocity *= get_friction();

    if (abs(velocity.x) < 0.0015f) {
        velocity.x = 0;
    }

    if (abs(velocity.y) < 0.0015f) {
        velocity.y = 0;
    }

    position += velocity;
}

void Character::update_spritesheet_frame(f32 deltaTime) {
    frameDelta += deltaTime;
    if (frameDelta >= SPRITESHEET_NEXT_FRAME_DELTA) {
        frameDelta -= SPRITESHEET_NEXT_FRAME_DELTA;

        spritesheetColumn += 1;
        spritesheetColumn %= 4;
    }

    if (velocity == glm::vec2 { 0, 0 }) {
        spritesheetColumn = 0;
    }
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

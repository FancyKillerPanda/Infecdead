#include "entities/player.hpp"
#include "game/game.hpp"
#include "graphics/vertex.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

constexpr const f32 SPRITESHEET_NEXT_FRAME_DELTA = 0.15f;

Player::Player(glm::vec2 position) : position(position) {
    init();
}

void Player::init() {
    if (initialised) {
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

    // NOTE(fkp): The order of items here must be the same as the order of enum variants in get_current_texture().
    spritesheetTextures.emplace_back("res/textures/characters/player/walk.png");

    log_::info("Initialised static Player data.");
    initialised = true;
}

void Player::update(f32 deltaTime) {
    // Rotation
    glm::vec2 mousePosition = Game::get().get_world_mouse_position();
    glm::vec2 mouseDirection = mousePosition - Game::get().to_view_space(position);
    f32 angle = -atan2(mouseDirection.y, mouseDirection.x);

    rotation = fmod(glm::degrees(angle) + 360.0f, 360.0f);
    f32 rotationRadians = glm::radians(rotation);
    glm::vec2 rotationVector = glm::normalize(glm::vec2 { cos(rotationRadians), -sin(rotationRadians) });

    // Movement
    const Uint8* keyboard = SDL_GetKeyboardState(nullptr);
    acceleration = { 0, 0 };

    if (keyboard[SDL_SCANCODE_W]) {
        acceleration += rotationVector * get_walk_acceleration();
    }
    if (keyboard[SDL_SCANCODE_S]) {
        acceleration += rotationVector * -get_walk_acceleration() * 0.5f;
    }
    if (keyboard[SDL_SCANCODE_A]) {
        acceleration += glm::vec2(rotationVector.y, -rotationVector.x) * get_walk_acceleration() * 0.5f;
    }
    if (keyboard[SDL_SCANCODE_D]) {
        acceleration += glm::vec2(-rotationVector.y, rotationVector.x) * get_walk_acceleration() * 0.5f;
    }

    velocity += acceleration * deltaTime;
    velocity *= get_friction();

    if (abs(velocity.x) < 0.0015f) {
        velocity.x = 0;
    }

    if (abs(velocity.y) < 0.0015f) {
        velocity.y = 0;
    }

    position += velocity;

    // Texture
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

void Player::render() {
    if (!initialised) {
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

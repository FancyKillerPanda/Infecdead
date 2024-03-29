#include "entities/player.hpp"
#include "game/game.hpp"
#include "graphics/vertex.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Player::Player(glm::vec2 position) : Character(position) {
    init();
}

void Player::init() {
    if (isInitialised) {
        return;
    }

    init_common();

    // NOTE(fkp): The order of items here must be the same as the order of enum variants in get_current_texture().
    spritesheetTextures.emplace_back("res/textures/characters/player/walk.png");

    log_::info("Initialised static Player data.");
    isInitialised = true;
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

    update_position(deltaTime);
    update_spritesheet_frame(deltaTime);
}

const Texture& Player::get_current_texture() {
    if (!isInitialised || spritesheetTextures.size() == 0) {
        log_::error("Attempting to get player texture before initialisation.");
        return Texture::missing();
    }

    // NOTE(fkp): The order of items here must be the same as the order of items in init_textures().
    enum SpritesheetState {
        WALK,
    };

    return spritesheetTextures[WALK];
}

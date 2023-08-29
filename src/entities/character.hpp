#pragma once

#include "common.hpp"
#include "graphics/texture.hpp"

#include <glm/glm.hpp>

#include <vector>

class Character {
public:
    Character() = default;
    Character(glm::vec2 position);
    virtual ~Character() = default;

    virtual void update(f32 deltaTime) {}
    virtual void render();

protected:
    static void init_common();

    void update_position(f32 deltaTime);
    void update_spritesheet_frame(f32 deltaTime);

    virtual const Texture& get_current_texture() = 0;

    virtual f32 get_walk_acceleration() { return 8.0f; }
    virtual f32 get_friction() { return 0.95f; }

protected:
    inline static bool isCommonInitialised = false;

    f32 rotation = 0;
    glm::vec2 position = { 0, 0 };
    glm::vec2 velocity = { 0, 0 };
    glm::vec2 acceleration = { 0, 0 };

    f32 frameDelta = 0;
    u32 spritesheetColumn = 0;

    inline static glm::vec2 dimensions;

    inline static u32 vao = 0;
    inline static u32 vbo = 0;
    inline static u32 ibo = 0;

    inline static u32 shader = 0;
    inline static u32 modelMatrixUniformLocation = 0;
    inline static u32 columnUniformLocation = 0;
    inline static u32 rotationUniformLocation = 0;
};

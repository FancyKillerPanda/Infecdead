#pragma once

#include "common.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#include <glm/glm.hpp>

#include <vector>

class Player {
public:
    Player() = default;
    Player(glm::vec2 position);

    void update(f32 deltaTime);
    void render();

private:
    static void init();
    const Texture& get_current_texture();

    f32 get_walk_acceleration() { return 8.0f; }
    f32 get_friction() { return 0.95f; }

private:
    inline static bool initialised = false;

    f32 rotation = 0;
    glm::vec2 position = { 0, 0 };
    glm::vec2 velocity = { 0, 0 };
    glm::vec2 acceleration = { 0, 0 };

    f32 frameDelta = 0;
    u32 spritesheetColumn = 0;

    inline static glm::vec2 dimensions;
    inline static std::vector<Texture> spritesheetTextures;

    inline static u32 vao = 0;
    inline static u32 vbo = 0;
    inline static u32 ibo = 0;

    inline static u32 shader = 0;
    inline static u32 modelMatrixUniformLocation = 0;
    inline static u32 columnUniformLocation = 0;
    inline static u32 rotationUniformLocation = 0;
};
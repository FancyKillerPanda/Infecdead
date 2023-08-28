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

    void update(f64 deltaTime);
    void render();

private:
    static void init();
    const Texture& get_current_texture();

private:
    inline static bool initialised = false;

    glm::vec2 position;
    f64 rotation = 0;
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
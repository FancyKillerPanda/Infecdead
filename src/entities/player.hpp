#pragma once

#include "common.hpp"
#include "entities/character.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#include <glm/glm.hpp>

#include <vector>

class Player : public Character {
public:
    Player() = default;
    Player(glm::vec2 position);

    void update(f32 deltaTime) override;

private:
    static void init();
    const Texture& get_current_texture() override;

private:
    inline static bool isInitialised = false;
    inline static std::vector<Texture> spritesheetTextures;
};
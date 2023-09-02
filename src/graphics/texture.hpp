#pragma once

#include "common.hpp"

#include <glm/glm.hpp>

#include <SDL3/SDL.h>

class Texture {
public:
    Texture() = default;
    Texture(const u8* filepath);
    Texture(SDL_Surface* surface);

    static Texture& missing();

    u32 get_id() const { return id; }
    const glm::vec2& get_dimensions() const { return dimensions; }

private:
    void init(SDL_Surface* surface);
    static void init_missing_texture();

    static void flip_surface_vertically(SDL_Surface* surface);

private:
    u32 id = 0;
    glm::vec2 dimensions;

    static Texture missingTexture;
};

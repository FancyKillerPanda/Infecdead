#include "graphics/texture.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <string.h>

Texture Texture::missingTexture {};

Texture::Texture(const u8* filepath) {
    SDL_Surface* surface = IMG_Load(filepath);
    if (!surface) {
        log_::error("Failed to load image '%s'.\n%s", filepath, SDL_GetError());
        id = Texture::missing().get_id();

        return;
    }

    init(surface);
    SDL_DestroySurface(surface);
}

Texture::Texture(SDL_Surface* surface) {
    init(surface);
}

void Texture::init(SDL_Surface* surface) {
    u32 format = 0;
    u32 formatBase = 0;
    if (surface->format->BytesPerPixel == 3) {
        format = GL_RGB8;
        formatBase = GL_RGB;
    } else if (surface->format->BytesPerPixel == 4) {
        format = GL_RGBA8;
        formatBase = GL_RGBA;
    } else {
        log_::error("Surface has unknown format.");
        id = Texture::missing().get_id();

        return;
    }

    flip_surface_vertically(surface);

    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(id, 1, format, surface->w, surface->h);
    glTextureSubImage2D(id, 0, 0, 0, surface->w, surface->h, formatBase, GL_UNSIGNED_BYTE, surface->pixels);
    // glGenerateTextureMipmap(id);
}

Texture& Texture::missing() {
    if (missingTexture.get_id() == 0) {
        init_missing_texture();
    }

    return missingTexture;
}

void Texture::init_missing_texture() {
    SDL_Surface* surface = SDL_CreateSurface(2, 2, SDL_PIXELFORMAT_RGBA8888);
    if (!surface) {
        log_::error("Could not create surface for missing texture.\n%s", SDL_GetError());
        return;
    }

    SDL_LockSurface(surface);
    ((u32*) surface->pixels)[0] = 0xffff00ff;
    ((u32*) surface->pixels)[1] = 0xff000000;
    ((u32*) surface->pixels)[2] = 0xff000000;
    ((u32*) surface->pixels)[3] = 0xffff00ff;
    SDL_UnlockSurface(surface);

    missingTexture.init(surface);
    SDL_DestroySurface(surface);

    log_::info("Initialised missing texture.");
}

// https://stackoverflow.com/questions/65815332/flipping-a-surface-vertically-in-sdl2
void Texture::flip_surface_vertically(SDL_Surface* surface) {
    SDL_LockSurface(surface);

    s32 pitch = surface->pitch; // Row size.
    u8* temp = new u8[pitch]; // Intermediate buffer.
    u8* pixels = (u8*) surface->pixels;

    for (s32 i = 0; i < surface->h / 2; i++) {
        // Get pointers to the two rows to swap.
        u8* row1 = pixels + (i * pitch);
        u8* row2 = pixels + ((surface->h - i - 1) * pitch);

        // Swap rows.
        memcpy(temp, row1, pitch);
        memcpy(row1, row2, pitch);
        memcpy(row2, temp, pitch);
    }

    delete[] temp;
    SDL_UnlockSurface(surface);
}

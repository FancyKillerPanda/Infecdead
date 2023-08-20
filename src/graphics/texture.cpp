#include "graphics/texture.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <SDL3_image/SDL_image.h>

#include <string.h>

Texture::Texture(const u8* filepath) {
    SDL_Surface* surface = IMG_Load(filepath);
    if (!surface) {
        log_::error("Failed to load image '%s'.\n%s", filepath, SDL_GetError());
        return;
    }

    init(surface);
    SDL_DestroySurface(surface);
}

Texture::Texture(SDL_Surface* surface) {
    init(surface);
}

void Texture::init(SDL_Surface* surface) {
    glCreateTextures(GL_TEXTURE_2D, 1, &id);

    glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
        return;
    }

    flip_surface_vertically(surface);

    glTextureStorage2D(id, 1, format, surface->w, surface->h);
    glTextureSubImage2D(id, 0, 0, 0, surface->w, surface->h, formatBase, GL_UNSIGNED_BYTE,
                        surface->pixels);
    glGenerateTextureMipmap(id);
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

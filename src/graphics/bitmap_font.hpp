#pragma once

#include "common.hpp"
#include "graphics/texture.hpp"

struct FontCharacter {
    s32 id = 0;

    u32 x = 0;
    u32 y = 0;
    u32 width = 0;
    u32 height = 0;

    s32 xOffset = 0;
    s32 yOffset = 0;

    s32 xAdvance = 0;
};

class BitmapFont {
public:
    BitmapFont() = default;
    BitmapFont(const u8* imageFilepath, const u8* descFilepath);

    const FontCharacter& operator[](s32 character) const;

    const Texture& get_texture() const { return texture; }

private:
    bool parse_bmfont_file(const u8* descFilepath);

private:
    bool initialised = false;

    Texture texture;
    FontCharacter characters[128];
};

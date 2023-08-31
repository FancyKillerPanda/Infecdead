#pragma once

#include "graphics/bitmap_font.hpp"

#include <glm/glm.hpp>

class Text {
public:
    Text(const BitmapFont& font, const u8* string);

    void render(glm::vec2 position);
};

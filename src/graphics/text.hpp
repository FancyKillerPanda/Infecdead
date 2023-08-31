#pragma once

#include "graphics/bitmap_font.hpp"
#include "graphics/shader.hpp"

#include <glm/glm.hpp>

class Text {
public:
    Text() = default;
    Text(const BitmapFont* font, const u8* string);

    void render(glm::vec2 position);

private:
    bool isInitialised = false;

    u32 vao = 0;
    const BitmapFont* font;

    inline static Shader shader;
};

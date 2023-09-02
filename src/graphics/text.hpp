#pragma once

#include "graphics/bitmap_font.hpp"
#include "graphics/shader.hpp"
#include "graphics/vertex.hpp"

#include <glm/glm.hpp>

#include <vector>

class Text {
public:
    Text() = default;
    Text(const BitmapFont* font, const u8* string);

    void render(glm::vec2 position);

private:
    bool isInitialised = false;

    u32 vao = 0;
    const BitmapFont* font;

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    inline static Shader shader;
    inline static u32 modelMatrixUniformLocation = 0;
};

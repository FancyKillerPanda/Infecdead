#include "graphics/text.hpp"
#include "graphics/vertex.hpp"

#include <string.h>
#include <vector>

Text::Text(const BitmapFont& font, const u8* string) {
    u32 stringLength = strlen(string);

    std::vector<Vertex> vertices;
    std::vector<u32> indices;
    vertices.reserve(stringLength * 4);
    indices.reserve(stringLength * 6);

    for (u32 i = 0; i < stringLength; i++) {
    }
}

void Text::render(glm::vec2 position) {
}

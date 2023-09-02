#include "graphics/text.hpp"
#include "graphics/vertex_array.hpp"
#include "utility/log.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string.h>

Text::Text(const BitmapFont* font, const u8* string) : font(font) {
    if (shader.get_id() == 0) {
        // Not yet initialised.
        shader = { "res/shaders/text.vert", "res/shaders/text.frag" };
        glProgramUniform1i(shader.get_id(), glGetUniformLocation(shader.get_id(), "texSampler"), 0);
        modelMatrixUniformLocation = glGetUniformLocation(shader.get_id(), "model");
    }

    u32 stringLength = strlen(string);
    if (stringLength == 0) {
        return;
    }

    vertices.reserve(stringLength * 4);
    indices.reserve(stringLength * 6);

    f32 x = (*font)[string[0]].xOffset;
    f32 y = font->base;

    for (u32 i = 0; i < stringLength; i++) {
        const FontCharacter& character = (*font)[string[i]];
        const glm::vec2& textureDimensions = font->get_texture().get_dimensions();
        f32 texCoordX_0 = character.x / textureDimensions.x;
        f32 texCoordY_0 = (textureDimensions.y - character.y - character.height) / textureDimensions.y;
        f32 texCoordX_1 = (character.x + character.width) / textureDimensions.x;
        f32 texCoordY_1 = (textureDimensions.y - character.y) / textureDimensions.y;

        f32 currentX = x - character.xOffset;
        f32 currentY = y - font->base + character.yOffset;

        vertices.emplace_back(Vertex { { currentX, currentY }, { texCoordX_0, texCoordY_1 } });
        vertices.emplace_back(Vertex { { currentX, currentY + character.height }, { texCoordX_0, texCoordY_0 } });
        vertices.emplace_back(
            Vertex { { currentX + character.width, currentY + character.height }, { texCoordX_1, texCoordY_0 } });
        vertices.emplace_back(Vertex { { currentX + character.width, currentY }, { texCoordX_1, texCoordY_1 } });

        indices.emplace_back((i * 4) + 0);
        indices.emplace_back((i * 4) + 1);
        indices.emplace_back((i * 4) + 2);
        indices.emplace_back((i * 4) + 2);
        indices.emplace_back((i * 4) + 3);
        indices.emplace_back((i * 4) + 0);

        x += character.xAdvance;
    }

    vao = VertexArray::from_data(vertices, indices);

    isInitialised = true;
}

void Text::render(glm::vec2 position) {
    if (!isInitialised) {
        log_::warn("Trying to render uninitialised text.");
        return;
    }

    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
    glProgramUniformMatrix4fv(shader.get_id(), modelMatrixUniformLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    glBindVertexArray(vao);
    shader.use();
    glBindTextureUnit(0, font->get_texture().get_id());
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

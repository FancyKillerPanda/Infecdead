#include "graphics/bitmap_font.hpp"
#include "utility/log.hpp"

#include <stdio.h>

BitmapFont::BitmapFont(const u8* imageFilepath, const u8* descFilepath) : texture(imageFilepath) {
    if (!parse_bmfont_file(descFilepath)) {
        return;
    }

    initialised = true;
}

bool BitmapFont::parse_bmfont_file(const u8* descFilepath) {
    if (initialised) {
        return true;
    }

    FILE* file = fopen(descFilepath, "r");
    if (!file) {
        log_::error("Could not open bitmap description file (%s).", descFilepath);
        return false;
    }

    u32 bufferLength = 1024;
    u8 buffer[bufferLength];

    // Info block line.
    s32 paddingUp = 0;
    s32 paddingRight = 0;
    s32 paddingDown = 0;
    s32 paddingLeft = 0;
    s32 spacingHorizontal = 0;
    s32 spacingVertical = 0;

    fgets(buffer, bufferLength, file);
    sscanf(buffer,
           R"(info face="%*[^"]" size=%*d bold=%*d italic=%*d charset="%*[^"]" unicode=%*d)"
           R"( stretchH=%*d smooth=%*d aa=%*d padding=%d,%d,%d,%d spacing=%d,%d)",
           &paddingUp, &paddingRight, &paddingDown, &paddingLeft, &spacingHorizontal, &spacingVertical);

    if (paddingUp + paddingDown + spacingVertical != 0 || paddingLeft + paddingRight + spacingHorizontal != 0) {
        // This is a custom requirement.
        log_::warn("Bitmap font (%s) has unequal padding/spacing.", descFilepath);
    }

    // Common block line.
    s32 pages = 0;

    fgets(buffer, bufferLength, file);
    sscanf(buffer, "common lineHeight=%*d base=%*d scaleW=%*d scaleH=%*d pages=%d packed=%*d", &pages);

    if (pages != 1) {
        // This is a custom requirement.
        log_::warn("Bitmap font (%s) has more than one page (pages: %d).", descFilepath, pages);
    }

    // Page block line.
    fgets(buffer, bufferLength, file);
    sscanf(buffer, R"(page id=%*d file="%*[^"]")");

    // Chars block line.
    fgets(buffer, bufferLength, file);
    sscanf(buffer, "chars count=%*d");

    // Characters.
    while (fgets(buffer, bufferLength, file)) {
        FontCharacter character {};
        sscanf(buffer, "char id=%d x=%u y=%u width=%u height=%u xoffset=%d yoffset=%d xadvance=%d page=%*d chnl=%*d",
               &character.id, &character.x, &character.y, &character.width, &character.height, &character.xOffset,
               &character.yOffset, &character.xAdvance);

        if (character.id < 0 || character.id > 127) {
            log_::warn("Ignoring character ID %d for bitmap font (%s).", (s32) character.id, descFilepath);
            continue;
        }

        characters[character.id] = character;
    }

    fclose(file);

    log_::info("Parsed bitmap font descriptor file (%s).", descFilepath);
    return true;
}

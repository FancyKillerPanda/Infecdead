#include "utility/log.hpp"

#include <stdio.h>

u8* read_entire_file(const u8* path) {
    FILE* file = fopen(path, "r");
    if (!file) {
        log::error("Failed to open file \"%s\".", path);
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    usize length = ftell(file);
    fseek(file, 0, SEEK_SET);

    u8* buffer = (u8*) calloc(length + 1, sizeof(u8));
    fread(buffer, sizeof(u8), length, file);
    fclose(file);

    return buffer;
}

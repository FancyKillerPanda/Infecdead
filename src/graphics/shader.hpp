#pragma once

#include "common.hpp"

class Shader {
public:
    Shader() = default;
    Shader(const u8* vertexPath, const u8* fragmentPath);

    void use();
    u32 get_id() const { return program; }

private:
    u32 program = 0;
};

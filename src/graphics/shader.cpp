#include "graphics/shader.hpp"
#include "utility/fileutils.hpp"

#include <glad/glad.h>

Shader::Shader(const u8* vertexPath, const u8* fragmentPath) {
    // Vertex shader.
    u8* vertexSource = read_entire_file(vertexPath);
    u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, nullptr);
    glCompileShader(vertexShader);

    // Fragment shader.
    u8* fragmentSource = read_entire_file(fragmentPath);
    u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
    glCompileShader(fragmentShader);

    // Shader program.
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glValidateProgram(program);

    // Cleanup.
    glDetachShader(program, fragmentShader);
    glDetachShader(program, vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);
}

void Shader::use() {
    glUseProgram(program);
}

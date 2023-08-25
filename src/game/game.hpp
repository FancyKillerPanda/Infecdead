#pragma once

#include "common.hpp"
#include "entities/player.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#include <SDL3/SDL.h>

class Game {
public:
    static Game& get();
    void init();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();

    Shader& get_basic_shader() {
        return basicShader;
    }
    Shader& get_spritesheet_shader() {
        return spritesheetShader;
    }

private:
    Game() = default;

    bool create_window();

    void handle_input();
    void update(f64 deltaTime);
    void render();

    static void debug_message_callback(u32 source, u32 type, u32 id, u32 severity, s32 length, const u8* message,
                                       const void* user);

private:
    bool running = false;
    SDL_Window* window = nullptr;
    SDL_GLContext context;

    Shader basicShader;
    Shader spritesheetShader;

    Player player;
};

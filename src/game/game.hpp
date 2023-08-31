#pragma once

#include "common.hpp"
#include "entities/player.hpp"
#include "graphics/bitmap_font.hpp"
#include "graphics/shader.hpp"
#include "graphics/texture.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>

// Used in internal calculations.
constexpr const glm::vec2 INTERNAL_WINDOW_DIMENSIONS = { 960.0f, 540.0f };

class Game {
public:
    static Game& get();
    void init();
    ~Game();

    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    void run();

    Shader& get_basic_shader() { return basicShader; }

    glm::vec2 get_world_mouse_position();

    glm::mat4 get_projection_matrix() const {
        return glm::ortho(0.0f, INTERNAL_WINDOW_DIMENSIONS.x, INTERNAL_WINDOW_DIMENSIONS.y, 0.0f);
    }

    glm::mat4 get_view_matrix() const { return glm::mat4(1.0f); }
    glm::vec2 to_view_space(glm::vec2 position);

private:
    Game() = default;

    bool create_window();

    void handle_input();
    void update(f32 deltaTime);
    void render();

    static void debug_message_callback(u32 source, u32 type, u32 id, u32 severity, s32 length, const u8* message,
                                       const void* user);

private:
    bool running = false;
    SDL_Window* window = nullptr;
    SDL_GLContext context;
    glm::vec2 windowDimensions; // TODO(fkp): Update when window size changes.

    u32 matricesUbo = 0;
    Shader basicShader;

    BitmapFont font;

    Player player;
};

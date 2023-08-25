#include "game/game.hpp"
#include "graphics/texture.hpp"
#include "graphics/vertex.hpp"
#include "utility/log.hpp"
#include "utility/utility.hpp"

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <chrono>
#include <string.h>

// TODO(fkp): Allow toggling between fullscreen and windowed.
bool FULLSCREEN = false;

// Number of milliseconds for each game tick.
f64 MS_PER_UPDATE = 10.0;

Game& Game::get() {
    static Game game;
    return game;
}

void Game::init() {
    if (!create_window()) {
        return;
    }

    running = true;
    log_::info("Game is running.");

    // Shaders
    basicShader = { "res/shaders/basic.vert", "res/shaders/basic.frag" };
    spritesheetShader = { "res/shaders/spritesheet.vert", "res/shaders/spritesheet.frag" };

    // Transformation
    glm::mat4 view { 1.0 };
    glm::mat4 projection = glm::ortho(0.0, 960.0, 540.0, 0.0);

    spritesheetShader.use();
    u32 location;
    location = glGetUniformLocation(spritesheetShader.get_program_id(), "view");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(view));
    location = glGetUniformLocation(spritesheetShader.get_program_id(), "projection");
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(projection));

    player = Player { glm::vec2(600.0, 200.0) };
}

Game::~Game() {
    SDL_DestroyWindow(window);
}

void Game::run() {
    std::chrono::high_resolution_clock::time_point previousTime = std::chrono::high_resolution_clock::now();
    f64 fpsDisplayDelta = 0.0;

    while (running) {
        std::chrono::high_resolution_clock::time_point currentTime = std::chrono::high_resolution_clock::now();
        f64 elapsedMs = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime).count() / 1000.0;
        previousTime = currentTime;

        handle_input();
        update(elapsedMs);
        render();

        fpsDisplayDelta += elapsedMs;
        if (fpsDisplayDelta > 500.0) {
            log_::info("%.2f FPS\n", 1000.0 / elapsedMs);
            fpsDisplayDelta = 0.0;
        }
    }
}

void Game::handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            running = false;
            log_::info("Quitting game.");

            break;
        }
    }
}

void Game::update(f64 deltaTime) {
}

void Game::render() {
    glClearColor(0.3, 0.0, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    player.render();

    SDL_GL_SwapWindow(window);
}

bool Game::create_window() {
    // Determines an appropriate window size.
    const SDL_DisplayMode* displayMode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());

    glm::vec2 windowDimensions;
    u32 windowFlags = SDL_WINDOW_OPENGL;

    if (FULLSCREEN) {
        windowDimensions = { displayMode->w, displayMode->h };
        windowFlags |= SDL_WINDOW_FULLSCREEN;
    } else {
        u32 size = min(displayMode->w / 16, displayMode->h / 9);
        size -= 10; // Makes the window a bit smaller than the screen size.
        windowDimensions = { size * 16.0f, size * 9.0f };

        if (windowDimensions.x > 1920 && windowDimensions.y > 1080) {
            windowDimensions = { 1920, 1080 };
        }
    }

    // Window OpenGL attributes.
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1); // Hardware acceleration.
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Creates the window object.
    window = SDL_CreateWindow("Infecdead", windowDimensions.x, windowDimensions.y, windowFlags);
    if (!window) {
        log_::error("SDL_CreateWindow() failed.\n%s", SDL_GetError());
        return false;
    }

    log_::info("Created SDL window.");

    context = SDL_GL_CreateContext(window);
    if (!context) {
        log_::error("SDL_GL_CreateContext() failed.\n%s", SDL_GetError());
        return false;
    }

    gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress);
    log_::info("Loaded OpenGL.\nVendor: %s\nRenderer: %s\nVersion: %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER),
               glGetString(GL_VERSION));

    if (!GLAD_GL_KHR_debug) {
        log_::error("OpenGL extension not available (GL_KHR_debug).");
        return false;
    }

    if (!GLAD_GL_ARB_direct_state_access) {
        log_::error("OpenGL extension not available (GL_ARB_direct_state_access).");
        return false;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debug_message_callback, nullptr);

    SDL_GL_SetSwapInterval(0);

    // Sets attributes for rendering.
    glViewport(0, 0, windowDimensions.x, windowDimensions.y);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

void Game::debug_message_callback(u32 source, u32 type, u32 id, u32 severity, s32 length, const u8* message, const void* user) {
    // Determines the appropriate output string for the source.
    const u8* sourceString = nullptr;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        sourceString = "OpenGL API";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        sourceString = "Window system";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        sourceString = "Shader compiler";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        sourceString = "Third party";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        sourceString = "Application";
        break;

    case GL_DEBUG_SOURCE_OTHER:
    default:
        sourceString = "Other";
        break;
    }

    // Determines the appropriate output string for the type.
    const u8* typeString = nullptr;

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        typeString = "Error";
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        typeString = "Deprecated behaviour";
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        typeString = "Undefined behaviour";
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        typeString = "Portability";
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        typeString = "Performance";
        break;
    case GL_DEBUG_TYPE_MARKER:
        typeString = "Marker";
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        typeString = "Push group";
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        typeString = "Pop group";
        break;

    case GL_DEBUG_TYPE_OTHER:
    default:
        typeString = "Other";
        break;
    }

    // Determines which logging function to use for this severity.
    using LogFunctionType = void (*)(const u8*, ...);
    LogFunctionType log_function = nullptr;

    switch (severity) {
    case GL_DEBUG_SEVERITY_NOTIFICATION:
    case GL_DEBUG_SEVERITY_LOW:
        log_function = log_::info;
        break;

    case GL_DEBUG_SEVERITY_MEDIUM:
        log_function = log_::warn;
        break;

    case GL_DEBUG_SEVERITY_HIGH:
        log_function = log_::error;
        break;
    }

    // Calculates the length of the message if it's not provided.
    if (length == -1) {
        length = strlen(message);
    }

    // Outputs the message.
    log_function("OpenGL debug callback. Source: \"%s\". Type: \"%s\". ID: %u.\nMessage: \"%.*s\".", sourceString, typeString,
                 id, length, message);
}

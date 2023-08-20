#include "common.hpp"
#include "game/game.hpp"
#include "utility/log.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <stdio.h>

s32 main(s32 argc, u8* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        log_::error("SDL_Init failed.\n%s", SDL_GetError());
        return 1;
    }

    log_::info("Initialised SDL.");

    s32 imgInitFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if (IMG_Init(imgInitFlags) != imgInitFlags) {
        log_::error("IMG_Init() failed.\n%s", SDL_GetError());
        return 1;
    }

    log_::info("Initialised SDL_image.");

    Game::get().run();

    IMG_Quit();
    log_::info("Quit SDL_image.");

    SDL_Quit();
    log_::info("Quit SDL.");

    return 0;
}

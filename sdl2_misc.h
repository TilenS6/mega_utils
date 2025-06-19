#include "SDL2/SDL.h"

bool handleQuitEvents(SDL_Event e) {
    if (e.type == SDL_QUIT) return true;
    if (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) return true;
    return false;
}
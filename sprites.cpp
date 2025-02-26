#include "sprites.h"

SDL2_helper::SDL2_helper(int w, int h, const char *title, const char *iconPath) {
    _W = w;
    _H = h;
    SDL_Init(SDL_INIT_VIDEO);
    _window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, 255);
    SDL_RenderClear(_renderer);
    SDL_RenderPresent(_renderer);
    SDL_Surface *icon = SDL_LoadBMP(iconPath);
    SDL_SetWindowIcon(_window, icon);
    SDL_FreeSurface(icon);
}
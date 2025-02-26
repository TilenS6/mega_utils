#pragma once
#include <iostream>
#include "SDL2/SDL.h"

class Camera {
public:
    SDL_Renderer *r = nullptr;
    double scale = 1, x = 0, y = 0;
    int w = 0, h = 0;

    void assignRenderer(SDL_Renderer *);
};

#include "camera.cpp"
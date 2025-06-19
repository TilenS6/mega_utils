#pragma once
#include <iostream>
#include "SDL2/SDL.h"

using namespace std;

#define CAM_DEFAULT_W 640
#define CAM_DEFAULT_H 480
#define CAM_DEFAULT_FLAGS (SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)

class Camera {
public:
    SDL_Renderer *r = nullptr;
    SDL_Window *wind = nullptr;
    double scale = 1, x = 0, y = 0;
    int w = 0, h = 0;

    Camera(int _w = CAM_DEFAULT_W, int _h = CAM_DEFAULT_H, const char *title = "Simply Good", const char *iconPath = nullptr, Uint32 flags = CAM_DEFAULT_FLAGS);
    ~Camera();
    int simplyInit(int _w = CAM_DEFAULT_W, int _h = CAM_DEFAULT_H, const char *title = "Simply Good", const char *iconPath = nullptr, Uint32 flags = CAM_DEFAULT_FLAGS);
    void assignWindow(SDL_Window *);
    void assignRenderer(SDL_Renderer *);
};

#include "camera.cpp"
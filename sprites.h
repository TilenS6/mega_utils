// Simple Game Engine
#pragma once
#include <iostream>
#include <vector>
#include "SDL2/SDL.h"

using namespace std;

class Sprite {
    SDL_Texture *_texture;
    SDL_Rect _rect, _srcRect;
    double _visibility;
    int _animationLoopType;
public:
    Sprite(int x, int y, int w, int h, const char *pathToTexture, double visibility = 1);
    ~Sprite();

    void setPos(int x, int y);
    void setSize(int w, int h);
    void move(int x, int y);
    void setVisibility(double newVisibility);

    // Animation
    void animate(int startX, int startY, int w, int h, int n, double t, int loopType); // w, h = width and height of one frame, n = number of frames, t = time between frames
    void setSrcRect(int x, int y, int w, int h);

    enum loopTypes {
        NO_LOOP = 0, // stand still frame
        LOOP = 1, // loop from start to end, then revert to 1st frame
        PING_PONG = 2, // loop from start to end, then from end to start
        ONCE = 3 // play once, then stand still at last frame
    };

    // Getters
    SDL_Rect getRect();
    SDL_Texture *getTexture();
};


#include "sprites.cpp"
#pragma once
#include <iostream>
#include "SDL2/SDL.h"

class Keyboard {
    const uint8_t n = 64;
    uint8_t arr[64]; // each key is single bit, marked by SDL_scancode.h
    uint8_t freshPressArr[64];
    char lastC;

public:
    Keyboard();
    void down(SDL_Scancode);
    void up(SDL_Scancode);
    void update(SDL_Event);

    bool get(SDL_Scancode);
    bool pressedNow(SDL_Scancode);
    char getLastChar();

    void newFrame();
};

#include "keyboard.cpp"
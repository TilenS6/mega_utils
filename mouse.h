#pragma once
#include <iostream>
#include "SDL2/SDL.h"
using namespace std;

class Mouse {
public:
    int lastX, lastY;
    int x, y;
    bool left = false, right = false, middle = false;
    Uint32 buttons = 0, lastButtons = 0;
    uint8_t last_event = 0;

    enum UpdateTypeMask {
        M_ButtonMask = 1,
        M_MovementMask = 2,
        M_LClickMask = 4,
        M_RClickMask = 8,
        M_MClickMask = 16,
    };

    uint8_t update();
};

#include "mouse.cpp"
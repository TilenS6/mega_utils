#include "mouse.h"

uint8_t Mouse::update() {
    uint8_t ret = 0;

    lastButtons = buttons;
    buttons = SDL_GetMouseState(&x, &y);
    left = buttons & SDL_BUTTON_LMASK;
    right = buttons & SDL_BUTTON_RMASK;
    if (buttons != lastButtons) ret |= M_ButtonMask;
    if (lastX != x || lastY != y) ret |= M_MovementMask;
    if (!(lastButtons & SDL_BUTTON_LMASK) && left) ret |= M_LClickMask;
    if (!(lastButtons & SDL_BUTTON_RMASK) && right) ret |= M_RClickMask;
    if (!(lastButtons & SDL_BUTTON_MMASK) && middle) ret |= M_MClickMask;

    lastX = x;
    lastY = y;
    last_event = ret;
    return ret;
}

Point Mouse::cameraToWorldPos(Camera* c) {
    Point p;
    p.x = (double)x / c->scale + c->x;
#ifdef LEGACY_COORDINATE_SYSTEM
    p.y = (double)(c->h - y) / c->scale + c->y;
#else
    p.y = (double)y / c->scale + c->y;
#endif
    return p;
}
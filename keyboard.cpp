#include "keyboard.h"

Keyboard::Keyboard() {
    for (int i = 0; i < n; i++)
        arr[i] = 0;
    lastC = '\0';
}

void Keyboard::down(SDL_Scancode s) {
    if (get(s)) return;
    arr[s / 8] |= (1 << (s % 8));
    freshPressArr[s / 8] |= (1 << (s % 8));
    const char *r = SDL_GetScancodeName(s);
    if (strlen(r) == 1) lastC = r[0];
}
void Keyboard::up(SDL_Scancode s) {
    arr[s / 8] &= ~(1 << (s % 8));
}
void Keyboard::update(SDL_Event e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
        down(e.key.keysym.scancode);
    else if (e.type == SDL_KEYUP)
        up(e.key.keysym.scancode);
}
bool Keyboard::get(SDL_Scancode s) {
    return (arr[s / 8] & (1 << (s % 8)));
}
bool Keyboard::pressedNow(SDL_Scancode s) {
    return (freshPressArr[s / 8] & (1 << (s % 8)));
}
void Keyboard::newFrame() {
    for (uint8_t i = 0; i < n; ++i)
        freshPressArr[i] = 0;
    lastC = '\0';
}
char Keyboard::getLastChar() {
    return lastC;
}
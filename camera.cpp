#include "camera.h"
void Camera::assignRenderer(SDL_Renderer *_r) {
    r = _r;
    SDL_GetRendererOutputSize(r, &w, &h);
}
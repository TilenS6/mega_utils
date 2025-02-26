#include "basicui.h"

BUI::BUI() {
    for (int i = 0; i < MAX_FONT_SIZE; ++i)
        Sans[i] = TTF_OpenFont(FONT_PATH, i);
}
void BUI::destroy() {
    for (int i = 0; i < MAX_FONT_SIZE; ++i)
        TTF_CloseFont(Sans[i]);
}
void BUI::assignCamera(Camera* _c) {
    c = _c;
}

SDL_Rect BUI::drawText(string text, int size, int x, int y, Uint8 r = 255, Uint8 g = 255, Uint8 b = 255) {
    SDL_Rect rect = { x, y, 0, 0 };
    if (size < 1 || size > MAX_FONT_SIZE || text.length() == 0) return rect;

    SDL_Surface* textSurface = TTF_RenderText_Blended(Sans[size], text.c_str(), SDL_Color({ (Uint8)r, (Uint8)g, (Uint8)b })); // use TTF_RenderText_Solid != TTF_RenderText_Blended for aliesed (stairs) edges
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(c->r, textSurface);
    SDL_QueryTexture(textTexture, NULL, NULL, &rect.w, &rect.h);

    SDL_RenderCopy(c->r, textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return rect;
}

string BUI::stringInput(string prompt) {
    bool running = true;
    SDL_Event event;
    string editBuffer = "";

    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_KEYDOWN:
            {
                if ((event.key.keysym.scancode >= SDL_SCANCODE_A && event.key.keysym.scancode <= SDL_SCANCODE_Z) || (event.key.keysym.scancode >= SDL_SCANCODE_1 && event.key.keysym.scancode <= SDL_SCANCODE_0) || event.key.keysym.scancode == SDL_SCANCODE_PERIOD) {
                    char c = *SDL_GetKeyName(event.key.keysym.sym);
                    editBuffer += c;
                } else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE || event.key.keysym.scancode == SDL_SCANCODE_SLASH) {
                    editBuffer += '_';
                }

            }
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_BACKSPACE:
                if (editBuffer.length() > 0) editBuffer.pop_back();
                break;

            case SDL_SCANCODE_ESCAPE:
            case SDL_SCANCODE_RETURN:
                running = false;
                break;

            default:
                break;
            }

            break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    c->w = event.window.data1;
                    c->h = event.window.data2;
                    cout << "resizan na " << event.window.data1 << ", " << event.window.data2 << endl;
                }
                break;
            default:
                break;
            }
        } // end: while (events)

        SDL_SetRenderDrawColor(c->r, 5, 5, 5, 255); // r b g a
        SDL_RenderClear(c->r);

        SDL_Rect r = drawText(prompt, 24, 0, 0);
        drawText(editBuffer, 24, 0, r.h + 6);

        SDL_RenderPresent(c->r);
    }
    return editBuffer;
}

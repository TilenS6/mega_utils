#include "camera.h"
void Camera::assignRenderer(SDL_Renderer *_r) {
    r = _r;
    SDL_GetRendererOutputSize(r, &w, &h);
}

void Camera::assignWindow(SDL_Window *_win) {
    wind = _win;
    SDL_GetWindowSize(wind, &w, &h);
}       

int Camera::simplyInit(int _w = CAM_DEFAULT_W, int _h = CAM_DEFAULT_H, const char *title = "Simply Good", const char *iconPath = nullptr, Uint32 flags = CAM_DEFAULT_FLAGS) {
    //PROFILE_FUNCTION();
    w = _w;
    h = _h;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "Error initializing SDL: " << SDL_GetError() << endl;
        return -1;
    }
    assignWindow(SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE)); // SDL_WINDOW_BORDERLESS nakonc (zraven SDL_WINDOW_RESIZABLE)
    if (!wind) {
        cout << "Error creating window: " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }
    assignRenderer(SDL_CreateRenderer(wind, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE));
    if (!r) {
        cout << "Error creating renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(wind);
        SDL_Quit();
        return -1;
    }

    TTF_Init();

    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
    if (SDL_GetError() == 0) {
        cout << "Error trought init: " << SDL_GetError() << endl;
        SDL_ClearError();
    }

    // TTF_Font *Sans = TTF_OpenFont("fonts/open-sans/OpenSans-Regular.ttf", 24);
    // SDL_Surface *textSurface;
    // textSurface = TTF_RenderText_Blended(Sans, "Hello world!", SDL_Color({255, 255, 255})); //use TTF_RenderText_Solid != TTF_RenderText_Blended for aliesed (stairs) edges
    // SDL_Texture *textTexture = SDL_CreateTextureFromSurface(r, textSurface);

    // icon

    if (iconPath != nullptr) {
        SDL_Surface *iconSurf = IMG_Load(iconPath);
        SDL_SetWindowIcon(wind, iconSurf);
    }
    return 0;
}

Camera::~Camera() {
    TTF_Quit();
    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(wind);
    SDL_Quit();
}
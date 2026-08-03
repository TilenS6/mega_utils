#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <vector>
using namespace std;

struct JoystickPosition {
    double direction;  // Angle in radians (0.0 to 2*PI)
    double magnitude;  // Distance from center (0.0 to 1.0)
};

class Controller {
    friend class Controllers;

    SDL_GameController* controller;
    SDL_JoystickID id;
    bool buttonsPressed[SDL_CONTROLLER_BUTTON_MAX] = {false};  // SDL_GameControllerButton
    int16_t axisValues[SDL_CONTROLLER_AXIS_MAX] = {0};         // SDL_GameControllerAxis
    int processingLevel = 2;  // 0: None, 1: Clamped magnitude, 2: + Deadzone
    double deadzone = 0.1;  // Deadzone value for processing level 2+

   public:
    Controller(SDL_GameController* _controller);

    SDL_JoystickID getID();
    void setProcessingLevel(int level);
    
    bool isButtonPressed(SDL_GameControllerButton button);
    int16_t getAxisValue(SDL_GameControllerAxis axis);
    JoystickPosition getJoystickPosition(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY);
    JoystickPosition getJoystickPositionLeft();
    JoystickPosition getJoystickPositionRight();
};

class Controllers {
    vector<Controller> controllers;

   public:
    Controllers();
    void eventUpdate(SDL_Event);

    vector<SDL_JoystickID> getControllerIDs();
    Controller* getControllerByID(SDL_JoystickID id);
    Controller* getControllerNum(int num);
};


#include "controller.cpp"

/* Example usage:

#include <iostream>
#include "mega_utils/utils_all.h"
using namespace std;

int main(int argc, char* argv[]) {
    Camera c;
    c.simplyInit();

    Keyboard kb;
    Controllers controllers;
    bool running = true;
    while (running) {
        SDL_Event event;
        kb.newFrame();
        while (SDL_PollEvent(&event)) {
            kb.eventUpdate(event);
            controllers.eventUpdate(event);
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        if (kb.get(SDL_SCANCODE_ESCAPE)) {
            running = false;
            break;
        }

        Controller* cont = controllers.getControllerNum(0);
        JoystickPosition pos;
        bool btnA = false;
        if (cont) {
            pos = cont->getJoystickPositionRight();
            btnA = cont->isButtonPressed(SDL_CONTROLLER_BUTTON_A);
        }

        SDL_SetRenderDrawColor(c.r, 0, 0, 0, 255);
        SDL_RenderClear(c.r);

        SDL_SetRenderDrawColor(c.r, 255, 255*(1-btnA), 255*(1-btnA), 255);
        SDL_RenderDrawLine(c.r, c.w/2, c.h/2, c.w/2 + 100 * cos(pos.direction), c.h/2 - 100 * sin(pos.direction));
        SDL_Rect rect = {c.w/2 - 100, c.h/2 - 100, 20, (int)(100.*pos.magnitude)};
        cout << pos.magnitude << endl;
        SDL_RenderFillRect(c.r, &rect);

        SDL_RenderPresent(c.r);
    }
    return 0;
}


*/
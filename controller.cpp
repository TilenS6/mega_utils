#include "controller.h"

Controller::Controller(SDL_GameController* _controller) {
    controller = _controller;
    id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
}

bool Controller::isButtonPressed(SDL_GameControllerButton button) {
    if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) {
        cout << "Invalid button index: " << button << endl;
        return false;  // Invalid button index
    }
    return buttonsPressed[button];
}
int16_t Controller::getAxisValue(SDL_GameControllerAxis axis) {
    if (axis < 0 || axis >= SDL_CONTROLLER_AXIS_MAX) {
        cout << "Invalid axis index: " << axis << endl;
        return 0;  // Invalid axis index
    }
    return axisValues[axis];
}

SDL_JoystickID Controller::getID() {
    return id;
}

void Controller::setProcessingLevel(int level) {
    if (level < 0 || level > 2) {
        cout << "Invalid processing level: " << level << endl;
        return;  // Invalid processing level
    }
    processingLevel = level;
}

JoystickPosition Controller::getJoystickPosition(SDL_GameControllerAxis axisX, SDL_GameControllerAxis axisY) {
    int16_t x = getAxisValue(axisX);
    int16_t y = getAxisValue(axisY);

    double magnitude = sqrt(x * x + y * y) / 32767.0;  // Normalize to [0, 1]
    if (processingLevel >= 1) {
        magnitude = min(magnitude, 1.0);  // Clamp magnitude to [0, 1]
    }
    double direction = atan2(-y, x);  // Convert to radians

    if (direction < 0) {
        direction += 2 * M_PI;  // Ensure direction is in [0, 2*PI)
    }

    if (processingLevel >= 2) { // Apply deadzone processing
        if (magnitude < deadzone) {
            magnitude = 0.0;
        } else {
            magnitude = (magnitude - deadzone) / (1.0 - deadzone);  // Scale to [0, 1]
        }
    }

    return {direction, magnitude};  // Clamp magnitude to [0, 1]
}

JoystickPosition Controller::getJoystickPositionLeft() {
    return getJoystickPosition(SDL_CONTROLLER_AXIS_LEFTX, SDL_CONTROLLER_AXIS_LEFTY);
}
JoystickPosition Controller::getJoystickPositionRight() {
    return getJoystickPosition(SDL_CONTROLLER_AXIS_RIGHTX, SDL_CONTROLLER_AXIS_RIGHTY);
}

// ----------- CONTROLLERS CLASS --------------

Controllers::Controllers() {
    // Find all connected controllers at initialization
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            cout << "Controller found: " << i << endl;
            controllers.push_back(Controller(SDL_GameControllerOpen(i)));
        }
    }
}

void Controllers::eventUpdate(SDL_Event event) {
    switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED: {
            bool found = false;
            for (const auto& controller : controllers) {
                if (controller.id == event.cdevice.which) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Controller added: " << event.cdevice.which << endl;
                controllers.push_back(Controller(SDL_GameControllerOpen(event.cdevice.which)));
            }
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED:
            cout << "Controller removed: " << event.cdevice.which << endl;
            for (auto it = controllers.begin(); it != controllers.end(); ++it) {
                if (it->controller && event.cdevice.which == it->id) {
                    SDL_GameControllerClose(it->controller);
                    controllers.erase(it);
                    break;
                }
            }
            break;
        case SDL_CONTROLLERDEVICEREMAPPED:
            for (auto& controller : controllers) {
                if (controller.controller && event.cdevice.which == controller.id) {
                    SDL_GameControllerClose(controller.controller);
                    controller.controller = SDL_GameControllerOpen(event.cdevice.which);
                    break;
                }
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            if (event.cbutton.button != -1) {
                for (auto& controller : controllers) {
                    if (controller.controller && event.cbutton.which == controller.id) {
                        controller.buttonsPressed[event.cbutton.button] = (event.type == SDL_CONTROLLERBUTTONDOWN);
                        break;
                    }
                }
            }
            break;

        case SDL_CONTROLLERAXISMOTION:
            for (auto& controller : controllers) {
                if (controller.controller && event.caxis.which == controller.id) {
                    controller.axisValues[event.caxis.axis] = event.caxis.value;
                    break;
                }
            }
            break;
    }
}

vector<SDL_JoystickID> Controllers::getControllerIDs() {
    vector<SDL_JoystickID> ids;
    for (const auto& controller : controllers) {
        ids.push_back(controller.id);
    }
    return ids;
}

Controller* Controllers::getControllerByID(SDL_JoystickID id) {
    for (auto& controller : controllers) {
        if (controller.id == id) {
            return &controller;
        }
    }
    throw std::runtime_error("Controller with the given ID not found.");
}

Controller* Controllers::getControllerNum(int num) {
    if (num < 0 || num >= controllers.size()) return nullptr;
    return &controllers[num];
}
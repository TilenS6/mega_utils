// Simple Game Engine
#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include "SDL2/SDL.h"
#include <nlohmann/json.hpp>
#include <regex>
#include "shapes.h"
#include <algorithm>
#include "timer.h"
#include "mouse.h"
#include "keyboard.h"
#include "controller.h"

using json = nlohmann::json;
using namespace std;

/*
File structure:
- Defaults: animation name = "idle", loop type = LOOP, fps = 10
- In shape of: <animation_name>_<frame#>.png
- Folder structure:
    pathToFolder
        meta.json
        idle_000.png
        idle_001.png
        ...

        animationName1_000.png
        animationName1_001.png
        animationName1_002.png
        ...
        animationName2_000.png
        animationName2_001.png
        ...

- Contains meta.json with structure:
{
    "idle": {
        "fps": <number>,
        "type": "NO_LOOP|LOOP|PING_PONG|ONCE"
    },
    "animationName1": {
        ...
    },
    ...
}
*/

struct SpriteEventDevices {
    Camera* c;
    Mouse* m;
    Keyboard* kb;
    Controllers* controllers;
    double dt;
};

class Sprite {
    typedef enum loopTypes {
        NO_LOOP = 0,    // stand still frame
        LOOP = 1,       // DEFAULT, loop from start to end, then revert to 1st frame
        PING_PONG = 2,  // loop from start to end, then from end to start
        ONCE = 3        // play once, then stand still at last frame
    } loopTypes;

    struct Animation {
        vector<SDL_Texture*> frameTextures;
        loopTypes loopType = LOOP;
        int fps = 10;
    };

    std::map<string, Animation> _animations;

    double _opacity = 1.;
    Point _offset = {0, 0};
    int _w, _h;
    double _scale = 1.;
    Sprite* _refferenceSprite = nullptr;

    // Animation
    Timer _animationTimer;
    string _currentAnimation = "idle";
    bool _animationStopped = false;
    double _stoppedTime = 0;  // For offseting animation
    Timer _stoppedTimer;      // For offseting animation

    Point getPos(Camera* c);
    void loadAnimations(SDL_Renderer* r, string pathToFolder);

   public:
    Sprite(SDL_Renderer* r, string pathToFolder, int x, int y, int w, int h, Sprite* refererenceSprite = nullptr, double opacity = 1);
    ~Sprite();

    inline void setOffset(double x, double y);
    inline void setOffset(Point p);
    inline void setScale(double scale);
    inline void moveBy(double x, double y);
    inline void moveBy(Point p);
    inline void setVisibility(double newVisibility);

    void startAnimation(string animationName);
    inline void restartAnimation();
    inline void pauseAnimation(bool pause = true);
    void changeAnimationLoopType(loopTypes newLoopType);

    void render(Camera* c);

    // Events;
    void updateEvents(Camera* c, Mouse* m, Keyboard* kb, Controllers* controllers, double dt);
    function<void(Sprite&, SpriteEventDevices)> onUpdate;
    function<void(Sprite&, SpriteEventDevices)> onClick;
    function<void(Sprite&, SpriteEventDevices)> onHover;
};

#include "sprite.cpp"
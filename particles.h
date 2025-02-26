#pragma once

#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include "mouse.h"
#include "keyboard.h"
#include "shapes.h"
#include "timer.h"
#include "FastCont.h"

using namespace std;

class Particle;
class ParticleS;

class Particle {
    Rectng r;
    Point vel;
    double vel_mult_second; // vel *= 1 - ((1 - vel_mult_second) * dt);
    double rem_life, init_life;
    SDL_Colour colour;

public:
    void init(Point, double, Point, double, double, unsigned char, unsigned char, unsigned char);
    bool update(double); // return: true=brisi sebe
    void render(Camera *);

    friend class ParticleS;
};

class ParticleS {
    double spwnTimer, spwnRate;
    double randDir, randSpeed, randLife; // vse +/-

    Point __spwnPnt;
    double __size;
    // Point __initial_vel;
    double __speed;
    double __dir;
    double __vel_mult_per_second;
    double __rem_life_seconds;
    unsigned char __red;
    unsigned char __grn;
    unsigned char __blu;

public:
    FastCont<Particle> ps;

    ParticleS();
    // void setInitial(Particle p);
    void setSpawnInterval(double);
    void setRandomises(double, double, double); // dir = absolute; life & speed = relative

    void moveSpawner(Point, double);

    void create(Point, double, double, double, double, double, unsigned char, unsigned char, unsigned char);
    void update(double, double, Point);
    void render(Camera *);
};

#include "particles.cpp"
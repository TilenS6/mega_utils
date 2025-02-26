#include "particles.h"
/*
    Rectng r;
    Point vel;
    double vel_mult_second; // vel
    SDL_Colour colour;
*/
void Particle::init(Point spwnPnt, double size, Point initial_vel, double vel_mult_per_second, double rem_life_seconds, unsigned char red, unsigned char grn, unsigned char blu) {
    r.a = spwnPnt;
    r.dimensions.x = size;
    r.dimensions.y = size;

    vel = initial_vel;
    vel_mult_second = vel_mult_per_second;
    rem_life = rem_life_seconds;
    init_life = rem_life_seconds;
    colour = { red, grn, blu, 255 };
}

bool Particle::update(double dt) {
    r.a += vel * dt;
    vel *= 1 - ((1 - vel_mult_second) * dt);
    rem_life -= dt;
    return rem_life <= 0;
}

void Particle::render(Camera *cam) {
    SDL_SetRenderDrawColor(cam->r, colour.r, colour.g, colour.b, 255 * (rem_life / init_life));
    r.render(cam);
}

// ------------------------ PS -------------------------
ParticleS::ParticleS() {
    spwnTimer = 0;
    spwnRate = 1;

    randDir = 0;
    randSpeed = 0;
    randLife = 0;

    ps.reserve_n_spots(256);
}

void ParticleS::create(Point spwnPnt, double size, double speed, double dir, double vel_mult_per_second, double rem_life_seconds, unsigned char red, unsigned char grn, unsigned char blu) {
    __spwnPnt = spwnPnt;
    __spwnPnt.x -= __size / 2;
    __spwnPnt.y += __size / 2;
    __size = size;
    __speed = speed;
    __dir = dir;
    __vel_mult_per_second = vel_mult_per_second;
    __rem_life_seconds = rem_life_seconds;
    __red = red;
    __grn = grn;
    __blu = blu;
}

void ParticleS::setSpawnInterval(double spawn_rate) {
    spwnRate = spawn_rate;
}
void ParticleS::setRandomises(double _randDir = 0, double _randSpeed = 0, double _randLife = 0) { // TODO+  random colour
    randDir = _randDir;
    randSpeed = _randSpeed;
    randLife = _randLife;
}
void ParticleS::moveSpawner(Point new_spwnPoint, double new_dir) {
    __spwnPnt = new_spwnPoint;
    // __spwnPnt.x -= __size / 2;
    // __spwnPnt.y += __size / 2;
    __dir = new_dir;
}

void ParticleS::update(double dt, double addMult = 1.0, Point relvel = { 0, 0 }) {
    spwnTimer += dt * addMult;

    while (spwnTimer >= spwnRate) {
        spwnTimer -= spwnRate;
        Particle tmpP;
        int id = ps.push_back(tmpP);

        // TODO randK je neki sussy

        double randK = ((rand() % 2001) * 0.001) - 1.0; // [-1, 1]
        randK += -sin(randK * PI) * .3; // more like x^3 graph; this *4 must be <.5
        double tmpDir = __dir + (randDir * randK);

        randK = ((rand() % 2001) * 0.001) - 1.0;
        randK += -sin(randK * PI) * .4;
        double tmpSpd = __speed + (randSpeed * __speed * randK);

        randK = ((rand() % 2001) * 0.001) - 1.0;
        randK += -sin(randK * PI) * .4;
        ps.at_id(id)->init(__spwnPnt, __size, { ((tmpSpd * cos(tmpDir)) + relvel.x) * addMult, ((tmpSpd * sin(tmpDir)) + relvel.y) * addMult }, __vel_mult_per_second, __rem_life_seconds + (randLife * __rem_life_seconds * randK), __red, __grn, __blu);
        // ps.at_id(id)->vel = {spd * cos(dir), spd * sin(dir)};
    }

    for (int i = 0; i < ps.size(); ++i) {
        if (ps.at_index(i)->update(dt)) { // zelja po izbrisu
            ps.remove_index(i);
            --i;
        }
    }
}

void ParticleS::render(Camera *cam) {
    /*
        Rectng r;
        r.a = __spwnPnt;
        r.dimensions = {__size, __size};
        SDL_SetRenderDrawColor(cam->r, 255, 255, 255, 255);
        r.render(cam);
    */

    for (int i = 0; i < ps.size(); ++i) {
        ps.at_index(i)->render(cam);
    }
}
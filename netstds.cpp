#pragma once

/*
initial (cel world, vse):
    FastCont<PhPoint> points; (id, pos, vel, mass)
    FastCont<PhLineObst> lineObst;
    FastCont<PhLink> links;
    FastCont<PhMuscle> muscles;
    FastCont<PhLinkObst> linkObst;
    FastCont<PhRocketThr> rocketThrs;
    FastCont<PhWeight> weights;
    FastCont<FuelCont> fuelConts;
    double gravity_accel;
    double vel_mult_second;



update (na vsake __ sek):
    FastCont<PhPoint> points; (pos, vel)
    FastCont<PhRocketThr> rocketThrs; (power)
    FastCont<PhWeight> weights; (added weight?)
    FastCont<FuelCont> fuelConts; (currentFuel)



FastCont<__>:
   st(uint32_T), [id, data],...



packet:
   HEADER - req/data, type
   BODY - data
   TRAILER - /

*/

enum {
    NETSTD_HEADER_REQUEST,
    NETSTD_HEADER_DATA,
};

enum {
    NETSTD_INIT,
    NETSTD_UPDATE_ALL,
    NETSTD_UPDATE_PLAYER_CONTROLS,
    NETSTD_PICKUP_ITEM,
    NETSTD_LOOT,
    NETSTD_DROP_ITEM,
    NETSTD_DELETE,
    NETSTD_ADD,
    NETSTD_BUILD,
    NETSTD_SITDOWN,
    NETSTD_STANDUP,
    NETSTD_PROJECTILE,
    NETSTD_INVENTORY,
    NETSTD_INVENTORY_DEMO,
    NETSTD_BYE,
};
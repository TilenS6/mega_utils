#pragma once
#include <iostream>
#include <vector>
#include "shapes.h"
using namespace std;

struct RenderTransform {
    int screenX, screenY;
    double scale;
};
#define isValidTransform(rt) (rt.scale > 0)

class Camera3D : public Camera {
public:
    double z = 0;
    double rotX = 0, rotY = 0, rotZ = 0;
    double zoom = 300, distance = 7;

    RenderTransform projectPoint(Point3 p);
    vector<RenderTransform> projectPoints(vector<Point3> pnts);

    void render3d(vector<Point3> pnts);
};

#include "camera3d.cpp"
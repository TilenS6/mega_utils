#pragma once
#include <iostream>
#include <vector>
#include "shapes.h"
using namespace std;

class Camera3D : public Camera {
public:
    double z = 0;
    double rotX = 0, rotY = 0, rotZ = 0;
    double zoom = 300, distance = 7;

    void render3d(vector<Point3> pnts);
};

#include "camera3d.cpp"
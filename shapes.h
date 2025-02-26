#pragma once
#include <iostream>
#include <math.h>
#include "camera.h"

#define PI 3.14159265358979323
#define hPI PI/2
#define PI2 PI*2

double map(double x, double in_min, double in_max, double out_min, double out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class Point {
public:
    double x, y;

    void render(Camera *);
    Point getRenderPos(Camera *);

    Point operator+(Point);
    Point operator-(Point);
    Point operator*(double);
    Point operator/(double);
    void operator+=(Point);
    void operator-=(Point);
    void operator+=(double);
    void operator-=(double);
    void operator*=(double);
    void operator/=(double);

    bool operator==(Point);
    bool operator!=(Point);
    void operator=(Point);
};

class Point3 {
public:
    double x, y, z;

    void render(Camera *);
    Point renderAt(Camera *);

    Point3 operator+(Point3);
    Point3 operator-(Point3);
    Point3 operator*(double);
    Point3 operator/(double);
    void operator+=(Point3);
    void operator-=(Point3);
    void operator+=(double);
    void operator-=(double);
    void operator*=(double);
    void operator/=(double);

    bool operator==(Point3);
    bool operator!=(Point3);
    void operator=(Point3);
};

class Line {
public:
    Point a, b;
    void render(Camera *);

    bool operator==(Line);
    bool operator!=(Line);
    void operator=(Line);
};

class Circle {
    double r;

public:
    double rPow2;
    Point a;
    void setRadius(double);
    double getRadius();
    void render(Camera *);
};

class Rectng {
public:
    Point a;          // top left corner
    Point dimensions; // (width/height), should be +
    SDL_Rect getRenderPos(Camera *);
    SDL_FRect getRenderPosF(Camera *);
    void render(Camera *);
};

#include "shapes.cpp"
#include "collisions.cpp"
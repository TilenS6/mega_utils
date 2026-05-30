#include "camera3d.h"

void Camera3D::render3d(vector<Point3> pnts) {
    Point3 c; //zacasna za res[]
    int size = pnts.size();
    Point3 res[size]; //rezultati tukaj
    Point3 raw[size]; //rezultati tukaj
    //face faces[nf];

    //kopiramo liste ker je poslan po referenci
    for (int i = 0; i < pnts.size(); i++) {
        res[i].x = pnts[i].x;
        res[i].y = pnts[i].y;
        res[i].z = pnts[i].z;
    }/*
    for (int i = 0; i < nf; i++) {
      faces[i].p1 = facesin[i].p1;
      faces[i].p2 = facesin[i].p2;
      faces[i].p3 = facesin[i].p3;
    }*/

    //stopinje v radiane
    double _rotX = rotX / 57.295779;
    double _rotY = rotY / 57.295779;
    double _rotZ = rotZ / 57.295779;

    //zracunamo in shranmo ker bomo rabl veckrat
    double cox = cos(_rotX);
    double six = sin(_rotX);

    double coy = cos(_rotY);
    double siy = sin(_rotY);

    double coz = cos(_rotZ);
    double siz = sin(_rotZ);

    //proceseramo tocke
    for (int i = 0; i < pnts.size(); i++) {
        // rotacija
        // TODO: rotacija okrog kamere, ne 0,0,0
        c.x = res[i].x;
        c.y = res[i].y;
        res[i].x = c.x * coz + c.y * -siz; // z
        res[i].y = c.x * siz + c.y * coz;

        c.y = res[i].y;
        c.z = res[i].z;
        res[i].y = c.y * cox + c.z * six; // x
        res[i].z = c.y * -six + c.z * cox;

        c.x = res[i].x;
        c.z = res[i].z;
        res[i].x = c.x * coy + c.z * siy; // y
        res[i].z = c.x * -siy + c.z * coy;

        //premiki
        res[i].x = res[i].x + x;
        res[i].y = res[i].y + y;
        res[i].z = res[i].z + z;

        //projekcija
        double z_res = 1 / (distance - res[i].z);
        raw[i].x = res[i].x;
        raw[i].y = res[i].y;
        raw[i].z = res[i].z;
        res[i].x = (raw[i].x * zoom) * z_res + (w / 2);
        res[i].y = (raw[i].y * zoom) * z_res + (h / 2);
    }

    /*
    sort(faces, res, nf);

    //brisemo screen
    tft.fillRect(lminx, lminy, lmaxx - lminx + 1, lmaxy - lminy + 1, BLACK);

    //renderer
    double area, k, color;
    for (int o = 0; o < nf; o++) {
        if (mat.t == 0) color = bgr(mat.r, mat.g, mat.b);
        else if (mat.t == 1) {
            area = areatriangle2d(raw[faces[o].p1 - 1].x, raw[faces[o].p1 - 1].y, raw[faces[o].p2 - 1].x, raw[faces[o].p2 - 1].y, raw[faces[o].p3 - 1].x, raw[faces[o].p3 - 1].y);
            k = area / angles[o];
            k = map2(31 * k, 0, 31, lights.envStrength, lights.camStrength);
            k = k / 31;
            k = max2(k, minDarkness);
            color = bgr(mat.r * k, mat.g * k, mat.b * k);
        } else color = bgr(11, 11, 11);
        //Serial.println(String(res[faces[o].p1 - 1].x) + ", " + String(res[faces[o].p1 - 1].y));
        tft.fillTriangle(res[faces[o].p1 - 1].x, res[faces[o].p1 - 1].y, res[faces[o].p2 - 1].x, res[faces[o].p2 - 1].y, res[faces[o].p3 - 1].x, res[faces[o].p3 - 1].y, color);
        if (mat.border == 1) tft.drawTriangle(res[faces[o].p1 - 1].x, res[faces[o].p1 - 1].y, res[faces[o].p2 - 1].x, res[faces[o].p2 - 1].y, res[faces[o].p3 - 1].x, res[faces[o].p3 - 1].y, WHITE);
    }
    */

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    for (int i = 0; i < pnts.size(); i++) {
        SDL_RenderDrawPoint(r, res[i].x, res[i].y);
    }
}
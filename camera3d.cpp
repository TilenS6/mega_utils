#include "camera3d.h"

RenderTransform Camera3D::projectPoint(Point3 p) {
    //koordinate v moj liking:
    // x = desno+, y = gor+, z = noter+
    p.y = -p.y;
    p.z = -p.z;

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

    RenderTransform res;
    // rotacija
    Point3 c;
    c = p;
    p.x = c.x * coz + c.y * -siz; // z
    p.y = c.x * siz + c.y * coz; 

    c = p;
    p.y = c.y * cox + c.z * six; // x
    p.z = c.y * -six + c.z * cox;

    c = p;
    p.x = c.x * coy + c.z * siy; // y
    p.z = c.x * -siy + c.z * coy;

    //premiki
    p.x = p.x + x;
    p.y = p.y + y;
    p.z = p.z + z;

    //projekcija
    Point3 raw;
    double z_res = 1 / (distance - p.z);
    raw.x = p.x;
    raw.y = p.y;
    raw.z = p.z;

    res.screenX = (raw.x * zoom) * z_res + (w / 2);
    res.screenY = (raw.y * zoom) * z_res + (h / 2);
    res.scale = z_res;

    return res;
}

vector<RenderTransform> Camera3D::projectPoints(vector<Point3> pnts) {
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

    vector<RenderTransform> res;
    res.reserve(pnts.size());

    for (int i = 0; i < pnts.size(); i++) {
        Point3 p = pnts[i];
        Point3 c;

        //koordinate v moj liking:
        // x = desno+, y = gor+, z = noter+
        p.y = -p.y;
        p.z = -p.z;

        // rotacija
        c = p;
        p.x = c.x * coz + c.y * -siz; // z
        p.y = c.x * siz + c.y * coz; 

        c = p;
        p.y = c.y * cox + c.z * six; // x
        p.z = c.y * -six + c.z * cox;

        c = p;
        p.x = c.x * coy + c.z * siy; // y
        p.z = c.x * -siy + c.z * coy;

        //premiki
        p.x = p.x + x;
        p.y = p.y + y;
        p.z = p.z + z;

        //projekcija
        Point3 raw;
        double z_res = 1 / (distance - p.z);
        raw.x = p.x;
        raw.y = p.y;
        raw.z = p.z;
        
        RenderTransform tmp;
        tmp.screenX = (raw.x * zoom) * z_res + (w / 2);
        tmp.screenY = (raw.y * zoom) * z_res + (h / 2);
        tmp.scale = z_res;
        res.push_back(tmp);
    }
    
    return res;
}

void Camera3D::render3d(vector<Point3> pnts) {
    vector<RenderTransform> projected = projectPoints(pnts);

    SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
    for (int i = 0; i < projected.size(); i++) {
        SDL_RenderDrawPoint(r, projected[i].screenX, projected[i].screenY);
    }
}
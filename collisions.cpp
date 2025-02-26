#include "shapes.h"

// LINE/LINE
bool collisionLineLine(Line l1, Line l2, Point *intersection = nullptr, double *ua = nullptr, double *ub = nullptr) {
    if (l1 == l2)
        return true;

    double uA = ((l2.b.x - l2.a.x) * (l1.a.y - l2.a.y) - (l2.b.y - l2.a.y) * (l1.a.x - l2.a.x)) / ((l2.b.y - l2.a.y) * (l1.b.x - l1.a.x) - (l2.b.x - l2.a.x) * (l1.b.y - l1.a.y));
    double uB = ((l1.b.x - l1.a.x) * (l1.a.y - l2.a.y) - (l1.b.y - l1.a.y) * (l1.a.x - l2.a.x)) / ((l2.b.y - l2.a.y) * (l1.b.x - l1.a.x) - (l2.b.x - l2.a.x) * (l1.b.y - l1.a.y));

    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        if (intersection != nullptr) {
            intersection->x = l1.a.x + (uA * (l1.b.x - l1.a.x));
            intersection->y = l1.a.y + (uA * (l1.b.y - l1.a.y));
        }
        return true;
    }
    return false;
}

// POINT/CIRCLE
bool collisionPointCircle(Point p, Circle c) {
    double dx = p.x - c.a.x;
    double dy = p.y - c.a.y;
    return (dx * dx + dy * dy) <= c.rPow2;
}

// LINE/CIRLCE
bool collisionLineCircle(Line l, Circle c, Point *closest = nullptr, double *dot_product = nullptr) {
    bool vRobovih = collisionPointCircle(l.a, c) || collisionPointCircle(l.b, c);
    if (vRobovih && closest == nullptr) return true;

    double distX = l.a.x - l.b.x;
    double distY = l.a.y - l.b.y;
    double lenPow2 = (distX * distX) + (distY * distY);

    // dot product
    double dot = (((c.a.x - l.a.x) * (l.b.x - l.a.x)) + ((c.a.y - l.a.y) * (l.b.y - l.a.y))) / lenPow2;
    if (dot_product != nullptr) *dot_product = dot;

    // najblizja tocka na crti
    double closestX = l.a.x + (dot * (l.b.x - l.a.x));
    double closestY = l.a.y + (dot * (l.b.y - l.a.y));

    if (closest != nullptr) {
        closest->x = closestX;
        closest->y = closestY;
    }
    if (vRobovih) return true;

    if (dot < 0 || dot > 1) return false;

    distX = closestX - c.a.x;
    distY = closestY - c.a.y;
    double distancePow2 = (distX * distX) + (distY * distY);

    return distancePow2 <= c.rPow2;
}

// CIRCLE/CIRCLE
bool collisionCircleCircle(Circle c1, Circle c2) {
    double dx = c1.a.x - c2.a.x;
    double dy = c1.a.y - c2.a.y;
    double distPow2 = dx * dx + dy * dy;
    double minDistPow2 = c1.getRadius() + c2.getRadius();
    minDistPow2 *= minDistPow2;

    return distPow2 <= minDistPow2;
}

// LINE/RECTANGLE
bool collisionLineRectangle(Line l, class Rectng b) {
    if (l.a.x > b.a.x && l.a.x < b.a.x + b.dimensions.x && l.a.y > b.a.y && l.a.y < b.a.y + b.dimensions.y) return true; // both points inside rect.
    Line tmp;

    // top
    tmp.a = b.a;
    tmp.b = {b.a.x + b.dimensions.x, b.a.y};
    if (collisionLineLine(l, tmp)) return true;
    // right
    tmp.a = {b.a.x + b.dimensions.x, b.a.y};
    tmp.b = {b.a.x + b.dimensions.x, b.a.y + b.dimensions.y};
    if (collisionLineLine(l, tmp)) return true;
    // bottom
    tmp.a = {b.a.x + b.dimensions.x, b.a.y + b.dimensions.y};
    tmp.b = {b.a.x, b.a.y + b.dimensions.y};
    if (collisionLineLine(l, tmp)) return true;
    // left
    tmp.a = {b.a.x, b.a.y + b.dimensions.y};
    tmp.b = b.a;
    if (collisionLineLine(l, tmp)) return true;

    return false;
}

// POINT/RECTANGLE
bool collisionPointRectangle(Point p, class Rectng b) {
    return (p.x >= b.a.x && p.x <= b.a.x + b.dimensions.x &&
            p.y >= b.a.y && p.y <= b.a.y + b.dimensions.y);
}

bool collisionPointInTriangle(Point point, Point a, Point b, Point c) {
    double as_x = point.x - a.x;
    double as_y = point.y - a.y;

    bool s_ab = ((b.x - a.x) * as_y - (b.y - a.y) * as_x) > 0;

    if ((((c.x - a.x) * as_y - (c.y - a.y) * as_x) > 0) == s_ab)
        return false;
    if ((((c.x - b.x) * (point.y - b.y) - (c.y - b.y) * (point.x - b.x)) > 0) != s_ab)
        return false;
    return true;
}
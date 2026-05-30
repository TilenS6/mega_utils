#include "shapes.h"

Point3 Point3::operator+(Point3 other) {
    return Point3{x + other.x, y + other.y, z + other.z};
}

Point3 Point3::operator-(Point3 other) {
    return Point3{x - other.x, y - other.y, z - other.z};
}

Point3 Point3::operator*(double value) {
    return Point3{x * value, y * value, z * value};
}

Point3 Point3::operator/(double value) {
    return Point3{x / value, y / value, z / value};
}

void Point3::operator+=(Point3 other) {
    x += other.x;
    y += other.y;
    z += other.z;
}

void Point3::operator-=(Point3 other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
}

void Point3::operator+=(double value) {
    x += value;
    y += value;
    z += value;
}

void Point3::operator-=(double value) {
    x -= value;
    y -= value;
    z -= value;
}

void Point3::operator*=(double value) {
    x *= value;
    y *= value;
    z *= value;
}

void Point3::operator/=(double value) {
    x /= value;
    y /= value;
    z /= value;
}

bool Point3::operator==(Point3 other) {
    return x == other.x && y == other.y && z == other.z;
}

bool Point3::operator!=(Point3 other) {
    return x != other.x || y != other.y || z != other.z;
}

void Point3::operator=(Point3 other) {
    x = other.x;
    y = other.y;
    z = other.z;
}


#include "smooth.h"

Smooth::Smooth() {
    value = 0;
    k = .2;
    valueWant = 0;
    t.interval();
}
Smooth::Smooth(double newK) {
    value = 0;
    valueWant = 0;
    setK(newK);
}
void Smooth::want(double setWant) {
    applyChanges();
    valueWant = setWant;
}
void Smooth::setK(double newK) {
    applyChanges();
    if (newK < 0) newK = 0;
    if (newK > 1) newK = 1;

    k = newK;
}

double Smooth::getVal() {
    applyChanges();
    return value;
}

// ----------------------

void Smooth::applyChanges() {
    double dt = t.interval();
    double tmp = pow(k, dt);

    value = value * tmp + valueWant * (1. - tmp);
}
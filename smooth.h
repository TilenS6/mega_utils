#pragma once
#include <iostream>
#include <math.h>
#include "timer.h"

class Smooth {
    double value;
    double valueWant;
    double k;  // koef. how much value remains after 1 sec.
    
    Timer t;

    void applyChanges();

public:
    Smooth();
    Smooth(double);
    void want(double);
    void setK(double);

    double getVal();
};

#include "smooth.cpp"
#include "timer.h"

Timer::Timer() {
    start = std::chrono::steady_clock::now();
    stop = start;
}

double Timer::interval() {
    start = std::chrono::steady_clock::now();
    double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop).count() * 0.000000001;
    stop = start;
    return dt;
}

double Timer::getTime() {
    start = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(start - stop).count() * 0.000000001;
}
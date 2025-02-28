#pragma once
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <string>
#include <mutex>

#define PROFILE_FUNCTION() ProfileScope profileScope(__func__)

class Profiler {
public:
    struct ProfileData {
        size_t callCount = 0;
        double totalTime = 0.0;
    };

    ~Profiler();

    static void record(const std::string &, double);
    static void report();
private:
    static std::unordered_map<std::string, ProfileData> &getData();
};

class ProfileScope {
    std::string functionName;
    std::chrono::high_resolution_clock::time_point start;
public:
    ProfileScope(const std::string &);
    ~ProfileScope();
};

#include "profiler.cpp"
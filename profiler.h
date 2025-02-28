#pragma once

#include <chrono>
#include <unordered_map>
#include <string>
#include <mutex>

#include <iostream>
#include <thread>
#include <sstream>
#include <cstdlib>
#include <unistd.h>


class Profiler {
public:
    struct ProfileData {
        size_t callCount = 0;
        double totalTime = 0.0;
    };

    static void record(const std::string& functionName, double duration);
    static void report();
    static void setMainThread();
    static bool isMainThread();
    static std::string resolveFunctionName(void* func); // 🔥 New function to get function names

private:
    static std::unordered_map<std::string, ProfileData>& getData();
};

extern "C" void __cyg_profile_func_enter(void* func, void* callsite);
extern "C" void __cyg_profile_func_exit(void* func, void* callsite);

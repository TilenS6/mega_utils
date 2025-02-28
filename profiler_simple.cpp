#include "profiler_simple.h"

Profiler::~Profiler() {
    report();
}

void Profiler::record(const std::string &functionName, double duration) {
    auto &data = getData()[functionName];
    data.callCount++;
    data.totalTime += duration;
}

void Profiler::report() {
    std::cout << "\n--- Profiling Report ---\n";
    for (const auto &[func, data] : getData()) {
        std::cout << func << " | Calls: " << data.callCount
            << " | Total Time: " << data.totalTime << "s"
            << " | Avg Time: " << (data.totalTime / data.callCount) << "s\n";
    }
}

std::unordered_map<std::string, Profiler::ProfileData> &Profiler::getData() {
    // static std::unordered_map<std::string, ProfileData> data;
    return data;
}

ProfileScope::ProfileScope(const std::string &funcName) : functionName(funcName) {
    start = std::chrono::high_resolution_clock::now();
}

ProfileScope::~ProfileScope() {
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    __profiler.record(functionName, duration.count());
}
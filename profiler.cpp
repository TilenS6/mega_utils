#include "profiler.h"

static thread_local std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
static std::mutex globalMutex;
static std::thread::id mainThreadId; // Stores main thread ID

void Profiler::setMainThread() {
    mainThreadId = std::this_thread::get_id();
}

bool Profiler::isMainThread() {
    return std::this_thread::get_id() == mainThreadId;
}

std::string Profiler::resolveFunctionName(void* func) {
    static std::unordered_map<void*, std::string> cache;
    static std::mutex cacheMutex;

    std::lock_guard<std::mutex> lock(cacheMutex);

    if (cache.find(func) != cache.end()) {
        return cache[func];
    }

    std::ostringstream command;
    command << "addr2line -e " << programName << " -f -p " << func;

    FILE* pipe = popen(command.str().c_str(), "r");
    if (!pipe) return "Unknown Function";

    char buffer[256];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    result.erase(result.find_last_not_of("\n") + 1);
    cache[func] = result;
    return result;
}

void Profiler::record(const std::string& functionName, double duration) {
    std::lock_guard<std::mutex> lock(globalMutex);

    auto& data = getData()[functionName];
    data.callCount++;
    data.totalTime += duration;
}

void Profiler::report() {
    std::cout << "\n--- Profiling Report (Depth 1) ---\n";
    for (const auto& [func, data] : getData()) {
        std::cout << func << " | Calls: " << data.callCount
                  << " | Total Time: " << data.totalTime << "s"
                  << " | Avg Time: " << (data.totalTime / data.callCount) << "s\n";
    }
}

std::unordered_map<std::string, Profiler::ProfileData>& Profiler::getData() {
    static std::unordered_map<std::string, ProfileData> data;
    return data;
}

// Function enter hook
extern "C" void __cyg_profile_func_enter(void* func, void* callsite) {
    (void)callsite; // Ignore callsite

    static thread_local int depth = 0;

    if (Profiler::isMainThread() && depth == 0) { // Only track depth 1 (functions called from main)
        std::string functionName = Profiler::resolveFunctionName(func);
        startTimes[functionName] = std::chrono::high_resolution_clock::now();
    }

    depth++; // Increase depth
}

// Function exit hook
extern "C" void __cyg_profile_func_exit(void* func, void* callsite) {
    (void)callsite; // Ignore callsite

    static thread_local int depth = 0;
    depth--; // Decrease depth

    if (Profiler::isMainThread() && depth == 0) {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::string functionName = Profiler::resolveFunctionName(func);

        if (startTimes.find(functionName) != startTimes.end()) {
            std::chrono::duration<double> elapsed = endTime - startTimes[functionName];
            Profiler::record(functionName, elapsed.count());
            startTimes.erase(functionName);
        }
    }
}

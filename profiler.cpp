#include "profiler.h"
#include <iostream>
#include <thread>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
    #define GET_PROGRAM_NAME() ([](){ char path[MAX_PATH]; GetModuleFileNameA(NULL, path, MAX_PATH); return std::string(path); }())
#else
    #include <unistd.h>
    #include <libgen.h> // For basename()
    extern char* program_invocation_name;
    #define GET_PROGRAM_NAME() (std::string(program_invocation_name))
#endif

std::string program_name = GET_PROGRAM_NAME();

std::unordered_map<std::string, size_t> profiler_call_count;
std::unordered_map<std::string, double> profiler_total_time;
std::mutex profiler_mutex;
std::thread::id profiler_main_thread_id;

void profiler_init() {
    profiler_main_thread_id = std::this_thread::get_id();
    std::cout << "[Profiler] Auto setup complete. Running profiling on " << program_name << "...\n";
}

bool profiler_is_main_thread() {
    return std::this_thread::get_id() == profiler_main_thread_id;
}

std::string profiler_resolve_function_name(void* func) {
    static std::unordered_map<void*, std::string> cache;
    static std::mutex cache_mutex;

    std::lock_guard<std::mutex> lock(cache_mutex);

    if (cache.find(func) != cache.end()) {
        return cache[func];
    }

    std::ostringstream command;
    command << "addr2line -e " << program_name << " -f -p " << func;

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

void profiler_record(const std::string& functionName, double duration) {
    std::lock_guard<std::mutex> lock(profiler_mutex);
    profiler_call_count[functionName]++;
    profiler_total_time[functionName] += duration;
}

void profiler_report() {
    std::cout << "\n--- Profiling Report (Depth 1) ---\n";
    for (const auto& [func, calls] : profiler_call_count) {
        double totalTime = profiler_total_time[func];
        std::cout << func << " | Calls: " << calls
                  << " | Total Time: " << totalTime << "s"
                  << " | Avg Time: " << (totalTime / calls) << "s\n";
    }
}

// Function enter hook
extern "C" void __cyg_profile_func_enter(void* func, void* callsite) { // TODO infinite call loop
    (void)callsite;

    static thread_local int depth = 0;
    static bool first_call = true;

    if (first_call) {
        const char* msg = "First function entered!\n";
        write(STDOUT_FILENO, msg, 23);  // Directly print without using C++ runtime
        first_call = false;
    } else if (profiler_is_main_thread() && depth == 0) {
        std::string functionName = profiler_resolve_function_name(func);
        static thread_local std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
        startTimes[functionName] = std::chrono::high_resolution_clock::now();
    }

    depth++;
}

// Function exit hook
extern "C" void __cyg_profile_func_exit(void* func, void* callsite) {
    (void)callsite;

    static thread_local int depth = 0;
    if (depth <= 0) return;
    depth--;

    if (profiler_is_main_thread() && depth == 0) {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::string functionName = profiler_resolve_function_name(func);

        static thread_local std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes;
        if (startTimes.find(functionName) != startTimes.end()) {
            std::chrono::duration<double> elapsed = endTime - startTimes[functionName];
            profiler_record(functionName, elapsed.count());
            startTimes.erase(functionName);
        }
    }
}

// 🔥 Report results automatically at program end
struct ProfilerFinalizer {
    ~ProfilerFinalizer() { profiler_report(); }
} __profiler_finalizer;

// Initialize profiler automatically
struct ProfilerInitializer {
    ProfilerInitializer() { profiler_init(); }
} __profiler_initializer;

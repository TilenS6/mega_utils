#pragma once

#include <chrono>
#include <unordered_map>
#include <string>
#include <mutex>

inline void profiler_init() __attribute__((no_instrument_function));
inline void profiler_record(const std::string& functionName, double duration) __attribute__((no_instrument_function));
inline void profiler_report() __attribute__((no_instrument_function));
inline bool profiler_is_main_thread() __attribute__((no_instrument_function));
inline std::string profiler_resolve_function_name(void* func) __attribute__((no_instrument_function));

extern "C" void __cyg_profile_func_enter(void* func, void* callsite) __attribute__((no_instrument_function));;
extern "C" void __cyg_profile_func_exit(void* func, void* callsite) __attribute__((no_instrument_function));;

extern std::string program_name;

// Include implementation at the end
#include "profiler.cpp"

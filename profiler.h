#pragma once

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sstream>
#include <string>
#include <libgen.h>	 // For basename()
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <dlfcn.h> // for f. addr. to name
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <algorithm> // For std::sort

using namespace std;

#ifdef _WIN32
#include <windows.h>
#define GET_PROGRAM_NAME() ([]() {             \
    char path[MAX_PATH];                       \
    GetModuleFileNameA (NULL, path, MAX_PATH); \
    return string (path); }())
#elif __linux__
extern char *program_invocation_name;
#define GET_PROGRAM_NAME() (string(program_invocation_name))
#else
#error "This code is not compatible with your OS!"
#endif

#ifndef PROFILER_SET_MAX_DEPTH
#define PROFILER_SET_MAX_DEPTH 1
#endif

bool disable_profiling = false;
thread_local int depth = -1;  // -1 means not yet in main

string program_name = GET_PROGRAM_NAME();

unordered_map<string, size_t> profiler_call_count;
unordered_map<string, double> profiler_total_time;
mutex profiler_mutex;

unordered_map<void *, string> cache;
mutex cache_mutex;


thread_local unordered_map<string, chrono::high_resolution_clock::time_point> startTimes;

int main(int argc, char *argv[]);

__attribute__((no_instrument_function)) void profiler_init();

__attribute__((no_instrument_function)) void profiler_record(const string &functionName, double duration);
__attribute__((no_instrument_function)) void profiler_report();

__attribute__((no_instrument_function)) string profiler_resolve_function_name(void *func);
__attribute__((no_instrument_function)) string get_program_base_address();


extern "C" __attribute__((no_instrument_function)) void __cyg_profile_func_enter(void *func, void *callsite);
extern "C" __attribute__((no_instrument_function)) void __cyg_profile_func_exit(void *func, void *callsite);

#include "profiler.cpp"

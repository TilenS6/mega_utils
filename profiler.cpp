#include "profiler.h"

extern string program_name;

// Function enter hook
extern "C" void __cyg_profile_func_enter(void *func, void *callsite) {
	(void)callsite;

	if (disable_profiling)
		return;

	disable_profiling = true;

	if (depth < 0) {
		if (func == main) {
			profiler_init();
		}
		depth += func == main;

		disable_profiling = false;
		return;
	}

	depth++;

	if (depth > PROFILER_SET_MAX_DEPTH) {
		disable_profiling = false;
		return;
	}

	string functionName = profiler_resolve_function_name(func);
	cout << functionName << endl;
	startTimes[functionName] = chrono::high_resolution_clock::now();

	disable_profiling = false;
}

// Function exit hook
extern "C" void __cyg_profile_func_exit(void *func, void *callsite) {
	(void)callsite;

	if (disable_profiling)
		return;

	if (depth < 0) {
		return;
	}

	disable_profiling = true;

	if (depth == 0) {  // Exited main
		profiler_report();
		depth--;
		disable_profiling = false;
		return;
	}

	auto endTime = chrono::high_resolution_clock::now();
	string functionName = profiler_resolve_function_name(func);

	if (startTimes.find(functionName) != startTimes.end()) {
		chrono::duration<double> elapsed = endTime - startTimes[functionName];
		profiler_record(functionName, elapsed.count());
		startTimes.erase(functionName);
	}

	depth--;

	disable_profiling = false;
}

void profiler_init() {
	cout << "[Profiler] Auto setup complete, hooks are working.\n";
	get_program_base_address();
}

string profiler_resolve_function_name(void *func) {
	lock_guard<mutex> lock(cache_mutex);

	if (cache.find(func) != cache.end()) {
		return cache[func];
	}

	ostringstream command;
	command << "addr2line -e " << program_name << " -f -p " << func;
	cout << "addr2line -e " << program_name << " -f -p " << func << endl;

	FILE *pipe = popen(command.str().c_str(), "r");
	if (!pipe)
		return "Unknown Function";

	char buffer[256];
	string result;
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}
	pclose(pipe);

	result.erase(result.find_last_not_of("\n") + 1);
	cache[func] = result;
	return result;
}

void profiler_record(const string &functionName, double duration) {
	lock_guard<mutex> lock(profiler_mutex);
	profiler_call_count[functionName]++;
	profiler_total_time[functionName] += duration;
}

void profiler_report() {
	cout << "\n--- Profiling Report (Max Depth = " << PROFILER_SET_MAX_DEPTH << ") ---\n";
	for (const auto &[func, calls] : profiler_call_count) {
		double totalTime = profiler_total_time[func];
		cout << func << " | Calls: " << calls
			 << " | Total Time: " << totalTime << "s"
			 << " | Avg Time: " << (totalTime / calls) << "s\n";
	}
}




#ifdef _WIN32
#error "This code is not yet compatible with Windows!"
#elif __linux__

string get_program_base_address() {
	ostringstream command;
	command << "cat /proc/self/maps";
	FILE *pipe = popen(command.str().c_str(), "r");
	if (!pipe)
		return "Unknown Function";

	char buffer[256];
	string result;
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}
	pclose(pipe);

	cout << result << endl;
	return string(result);
}

#define GET_PROGRAM_BASE_ADDRESS
#else
#error "This code is not compatible with your OS!"
#endif
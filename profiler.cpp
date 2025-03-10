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
	// profiler_resolve_function_name((void *)main);
}

string profiler_resolve_function_name(void *func) {
	/*
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
	*/

	// backtrace_symbols_fd(&func, 1, STDOUT_FILENO);
	// printf("%p", func);

	string backtrace = *backtrace_symbols(&func, 1);
	// cout << backtrace << endl;

	// TODOOO: use command "nm main" to get all function addresses and names

	// Extracting offset from string
	/*  /home/tilens/Documents/Projects/SDL2-mega_utils-Template/bin/main(+0x5b00) [0x555555559b00]  ->  5b00 */
	/*  /home/tilens/Documents/Projects/SDL2-mega_utils-Template/bin/main(__cyg_profile_func_exit+0) [0x55555555eb90]  -> __cyg_profile_funct_exit+0 */


	// program_name = /home/tilens/Documents/Projects/SDL2-mega_utils-Template/bin/main
	backtrace = backtrace.substr(program_name.size() + 1);
	size_t start = backtrace.find("(", 0) + 1;
	size_t end = backtrace.find(")", start);
	string offset = backtrace.substr(start, end - start);
	// cout << offset << endl;

	// check if offset contains only hex characters
	if (offset.find_first_not_of("+x0123456789abcdef") != string::npos) {
		return offset; // this is already a function name approximation
	}

	// Extracting function name from offset
	ostringstream command;
	command << "addr2line -e " << program_name << " -f -p -C " << offset;
	FILE *pipe = popen(command.str().c_str(), "r");
	if (!pipe)
		return "Unknown Function";

	char buffer[256];
	string result;
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}
	pclose(pipe);

	//cout << "addr2line -e " << program_name << " -f -p -C " << offset << endl;
	//cout << result << endl;

	// erase everything that path and program_name has in common from "at" onwards
	start = result.find("at", 0) + 3;
	string out = result.substr(0, start);

	size_t program_name_i = 0;
	for (size_t i = start; i < result.size(); i++) {
		if (program_name[program_name_i] != result[i]) {
			out += result.substr(i);
			break;
		}
		++program_name_i;
	}

	cout << "out: " << out << endl;

	return out;
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

string get_program_base_address(void *func) {  // !! not working yet
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
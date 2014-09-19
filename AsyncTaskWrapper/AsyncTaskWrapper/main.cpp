#include<iostream>

#include "AsyncTaskWrapper.h"
#include <functional>
#include <cstdarg>
#include <thread>
#include <chrono>

int sleeper(int length) {
	std::this_thread::sleep_for(std::chrono::milliseconds{ length });

	return length;
}

void async_task_wrapper() {
	std::cout << "Async Task Wrapper ----------------\n";

	AsyncTaskWrapper<int> atw([]() -> int { return 42; });
	std::cout << "Simple Return: " << atw.GetResultBlocking() << "\n";
	
	AsyncTaskWrapper<int, int> atw2([](int input) -> int { return input; }, 123);
	std::cout << "Input, output: " << atw2.GetResultBlocking() << "\n";

	AsyncTaskWrapper<int, int> atw_sleep(sleeper, 2000);
	std::cout << "Starting Sleeper\n";
	atw_sleep.Start();
	while (!atw_sleep.IsDone()) {
		std::cout << "Is Done?: " << std::boolalpha << atw_sleep.IsDone() << "\n";

		std::this_thread::sleep_for(std::chrono::milliseconds{ 500 });
	}
	std::cout << "Is Done?: " << std::boolalpha << atw_sleep.IsDone() << "\n";
	std::cout << "Result: " << atw_sleep.GetResult() << "\n";

}


int main(int argc, char*argv) {
	async_task_wrapper();

	char blank;
	std::cin >> blank;
}
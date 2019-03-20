// This program is a multi-threaded "call from" program in C++
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <thread>
#include <mutex>

// Mutex for critical section
std::mutex mtx;

// Initial function for each thread
void call_from(int tid) {
	mtx.lock();
	std::cout << "Launched by thread " << tid << std::endl;
	mtx.unlock();
}

int main() {
	// Create an array of 10 thread objects
	std::thread t[10];

	// Launch 10 threads with initial function "call_from(...)"
	for (int i = 0; i < 10; i++) {
		t[i] = std::thread(call_from, i);
	}

	// Print from main function
	mtx.lock();
	std::cout << "Launched from main" << std::endl;
	mtx.unlock();

	// Wait for all threads to complete
	for (int i = 0; i < 10; i++) {
		t[i].join();
	}

	return 0;
}
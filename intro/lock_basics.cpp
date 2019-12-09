// This program is a multi-threaded "call from" program in C++
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <mutex>
#include <thread>

using std::mutex;
using std::thread;
using std::cout;
using std::endl;

// Mutex for critical section
mutex mtx;

// Initial function for each thread
void call_from(int tid) {
  mtx.lock();
  cout << "Launched by thread " << tid << endl;
  mtx.unlock();
}

int main() {
  // Create an array of 10 thread objects
  thread t[10];

  // Launch 10 threads with initial function "call_from(...)"
  for (int i = 0; i < 10; i++) {
    t[i] = thread(call_from, i);
  }

  // Print from main thread
  call_from(10);

  // Wait for all threads to complete
  for (int i = 0; i < 10; i++) {
    t[i].join();
  }

  return 0;
}

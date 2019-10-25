// This program is a multi-threaded "call from" program in C++
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <thread>

using std::cout;
using std::endl;
using std::thread;

// Initial function for each thread
void call_from(int tid) {
  cout << "Launched by thread " << tid << endl;
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

  // Wait for all threads to complete (in order)
  for (int i = 0; i < 10; i++) {
    t[i].join();
  }

  return 0;
}

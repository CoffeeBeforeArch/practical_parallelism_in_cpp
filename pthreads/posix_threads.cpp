// This program shows off multithreading basics using pthreads
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <iostream>

using namespace std;

#define NUM_THREADS 10

// Global pthread implementation of mutex lock
pthread_mutex_t mtx;

// Argument we will cast as void* for your thread function
struct Args {
  int tid;
  int data;
};

// pthread functions must have return type void*, with a void* arg
void *boring_thread_function(void *args) {
  Args *local_args = (Args *)args;
  // Get the lock before entering the critical section
  pthread_mutex_lock(&mtx);
  cout << "Printing from thread " << local_args->tid << endl;
  cout << "Value of data is " << local_args->data << endl;
  // Release the lock to someone else
  pthread_mutex_unlock(&mtx);
}

int main() {
  // Create an array of 10 threads
  pthread_t threads[NUM_THREADS];
  Args per_thread_args[NUM_THREADS];

  // Create 10 threads
  for (int i = 0; i < NUM_THREADS; i++) {
    per_thread_args[i].tid = i;
    per_thread_args[i].data = i * i;
    // Launch the threads
    /*
        Arguments:
            1.) Pointer to pthread_t
            2.) Attributes for the thread (NULL means default)
            3.) Entry routine
            4.) Arguments
    */
    pthread_create(&threads[i], NULL, boring_thread_function,
                   (void *)&per_thread_args[i]);
  }

  // Return code for the thread
  void *ret;
  // Wait for all threads to finish before exiting the program
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(threads[i], &ret);
  }

  return 0;
}

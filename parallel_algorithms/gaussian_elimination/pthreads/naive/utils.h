// This file contains utility functions for the pthread parallel
// Gaussian Elimination
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <chrono>
#include "../../common/common.h"

using namespace std::chrono;

struct Args {
    // First row assigned to this thread
    int start_row;
    // One past the last row for this thread
    int end_row;
    // Matrix of floating point numbers
    float *matrix;
    // Dimensions of the square matrix
    int N;
    // Barrier to synchronize at
    pthread_barrier_t *barrier;
    // Variables needed for timing
    int num_threads;
    int *counter;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
    high_resolution_clock::time_point *start;
    high_resolution_clock::time_point *end;
};

void perf_cycle(int num_threads, int *counter, pthread_mutex_t *mtx,
        pthread_cond_t *cond,
        high_resolution_clock::time_point *time){
    // Get the lock
    pthread_mutex_lock(mtx);

    // Atomically decrement number of outstanding threads
    *counter -= 1;
    // Check if we are the last thread
    // If not, wait to be signaled
    if(*counter == 0){
        // Update a timing variable
        *time = high_resolution_clock::now();

        // Reset the counter
        *counter = num_threads;

        // Signal everyone to continue
        pthread_cond_broadcast(cond);
    }else{
        // Wait for the last thread before continuing
        pthread_cond_wait(cond, mtx);
    }

    // Everyone unlocks
    pthread_mutex_unlock(mtx);
}

// Pthread function for computing Gaussian Elimination
// Takes a pointer to a struct of args as an argument
void *ge_parallel(void *args){
    // Cast void pointer to struct pointer
    Args *local_args = (Args*)args;

    // Unpack the arguments
    int start_row = local_args->start_row;
    int end_row = local_args->end_row;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    pthread_barrier_t *barrier = local_args->barrier;

    int num_threads = local_args->num_threads;
    int *counter = local_args->counter;
    pthread_mutex_t *mtx = local_args->mtx;
    pthread_cond_t *cond = local_args->cond;
    high_resolution_clock::time_point *start = local_args->start;
    high_resolution_clock::time_point *end = local_args->end;

    // Wait for all threads to be created before profiling
    perf_cycle(num_threads, counter, mtx, cond, start);

    // Loop over all rows in the matrix
    for(int i = 0; i < N - 1; i++){
        // Check if pivot row belongs to this thread
        if((i >= start_row) && (i < end_row)){
            // Normalize this row to the pivot
            float pivot = matrix[i * N + i];

            // Loop over remaining elements in the pivot row
            for(int j = i + 1; j < N; j++){
                matrix[i * N + j] /= pivot;
            }

            // Use assignment for trivial case
            matrix[i * N + i] = 1;
        }

        // All threads must wait for pivot before continuing
        pthread_barrier_wait(barrier);

        // Loop over the rest of the rows to eliminate the ith element
        for(int j = i + 1; j < end_row; j++){
            // Check if row belongs to this thread
            if((j >= start_row) && (j < end_row)){
                // Scale the subtraction by the ith element of this row
                float scale = matrix[j * N + i];

                // Subtract from each element of the row
                for(int l = i + 1; l < N; l++){
                    matrix[j * N + l] -= matrix[i * N + l] * scale; 
                }

                // Use assignment for trivial case
                matrix[j * N + i] = 0;
            }
        }
    }

    // Handle trivial last row with only 1 element
    if((N - 1) >= start_row){
        matrix[N-1 * N + N - 1] = 1;
    }

    // Stop monitoring when last thread exits
    perf_cycle(num_threads, counter, mtx, cond, end);

    return 0;
}

// Helper function create thread 
void launch_threads(int num_threads, float* matrix, int N){

    // Create array of thread objects we will launch
    pthread_t threads[num_threads];

    // Create a barrier and initialize it
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, num_threads);

    // Create an array of structs to pass to the threads
    Args thread_args[num_threads];

    // Create variables for performance monitoring
    int counter = num_threads;
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;

    // Launch threads
    for(int i = 0; i < num_threads; i++){
        // Pack struct with its arguments
        thread_args[i].start_row = i * (N / num_threads);
        thread_args[i].end_row = i * (N / num_threads) + (N / num_threads);
        thread_args[i].matrix = matrix;
        thread_args[i].N = N;
        thread_args[i].barrier = &barrier;

        thread_args[i].num_threads = num_threads;
        thread_args[i].counter = &counter;
        thread_args[i].mtx = &mtx;
        thread_args[i].cond = &cond;
        thread_args[i].start = &start;
        thread_args[i].end = &end;

        // Launch the thread
        pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]);
    }

    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }

    // Cast timers as double to print
    duration<double> elapsed = duration_cast<duration<double>>(end - start);

    // Print out the elapsed time
    cout << "Elapsed time parallel = " << elapsed.count() << " seconds" <<  endl;
}


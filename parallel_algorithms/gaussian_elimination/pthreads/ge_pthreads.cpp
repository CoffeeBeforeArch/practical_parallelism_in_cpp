// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <stdlib.h>
#include "../common/common.h"

struct Args {
    // Number of threads in the program
    int num_threads;
    // First row assigned to this thread
    int start_row;
    // One past the last row for this thread
    int end_row;
    // Matrix of floating point numbers
    float *matrix;
    // Dimension of the square matrix
    int N;
    // Counter used to track arrival of threads
    int *counter;
    // Mutex lock to ensure counter updated atomically
    pthread_mutex_t *mtx;
    // Condition for threads to wait on
    pthread_cond_t *cond;
};

// Pthread function for computing Gaussian Elimination
// Takes a pointer to a struct of args as an argument
void *ge_parallel(void *args){
    // Cast void pointer to struct pointer
    Args *local_args = (Args*)args;

    // Unpack the arguments
    int num_threads = local_args->num_threads;
    int start_row = local_args->start_row;
    int end_row = local_args->end_row;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    int *counter = local_args->counter;
    pthread_mutex_t *mtx = local_args->mtx;
    pthread_cond_t *cond = local_args->cond;

    for(int i = 0; i < N; i++){
        // Only one thread should do pivot calculations at a time
        if((i >= start_row) && (i < end_row)){
            // Normalize this row to the pivot
            float pivot = matrix[i * N + i];
            for(int j = i + 1; j < N; j++){
                matrix[i * N + j] /= pivot;
            }

            // Use assignment for trivial case
            matrix[i * N + i] = 1;
        }

        // Last thread resets the counter in the critical section
        pthread_mutex_lock(mtx);

        // Decrement threads as they arrive
        *counter -= 1;

        // We are the last thread
        if(*counter == 0){
            *counter = num_threads;
            // Broadcast the condition to waiting threads
            pthread_cond_broadcast(cond);
        }else{
            // Go to sleep until condition is broadcasted
            pthread_cond_wait(cond, mtx);
        }

        // Unlock at the end of the critical section
        pthread_mutex_unlock(mtx);

        // Eliminate the ith column from the rest of the rows
        for(int j = i + 1; j < end_row; j++){
            // Check if row belongs to this thread
            if((j >= start_row) && (j < end_row)){
                // Scale the subtraction by the first element of this row
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

    return 0;
}

int main(){
    // Number of threads to launch
    int num_threads = 8;

    // Create array of thread objects we will launch
    pthread_t threads[num_threads];

    // Create a mutex lock and initialize it
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

    // Create a condition and initialize it
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

    // Create an array of structs to pass to the threads
    Args thread_args[num_threads];

    // Number of elements in square matrix
    int N = 8;

    // Declare our problem matrices
    float *matrix;
    float *matrix_pthread;

    // Declare and initialize the size of the matrix
    size_t bytes = N * N * sizeof(float);

    // Allocate space for our matrices
    matrix = (float*)malloc(bytes);
    matrix_pthread = (float*)malloc(bytes);
   
    // Create our progress counter for the threads
    int *counter = new int(num_threads);

    // Initialize a matrix and copy it
    init_matrix(matrix, N);
    memcpy(matrix_pthread, matrix, bytes);
    
    // Launch threads
    for(int i = 0; i < num_threads; i++){
        // Pack struct with its arguments
        thread_args[i].start_row = i * (N / num_threads);
        thread_args[i].end_row = i * (N / num_threads) + (N / num_threads);
        thread_args[i].num_threads = num_threads;
        thread_args[i].matrix = matrix_pthread;
        thread_args[i].N = N;
        thread_args[i].counter = counter;
        thread_args[i].mtx = &mtx;
        thread_args[i].cond = &cond;
        
        // Launch the thread
        pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]);
    }
   
    // Call the serial version in parallel
    ge_serial(matrix, N);

    // Join the threads as they complete
    void *ret;
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], &ret);
    }
  
    // Verify the solution
    verify_solution(matrix, matrix_pthread, N);

    return 0;
}

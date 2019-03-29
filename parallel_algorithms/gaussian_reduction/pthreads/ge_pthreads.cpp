// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <assert.h>

using namespace std;

struct Args {
    // Thread ID
    int tid;
    int num_threads;
    float *matrix;
    int N;
    pthread_mutex_t *mtx;
    pthread_cond_t *cond;
    int *counter;
};

// Serial function for computing Gaussian Elimination
// Takes a pointer to a matrix and its dimension as arguments 
void ge_serial(float *matrix, int n){
    // Iterate over each row in the matrix
    float pivot;
    for(int i = 0; i < n; i++){
        // Pivot will be the diagonal
        pivot = matrix[i * n + i];

        // Iterate of the remaining row elements
        for(int j = i + 1; j < n; j++){
            // Divide by the pivot
            matrix[i * n + j] /= pivot;
        }

        // Do direct assignment for trivial case (self-divide)
        matrix[i * n + i] = 1.0;

        // Eliminate ith element from the jth row
        float scale;
        for(int j = i + 1; j < n; j++){
            // Factor we will use to scale subtraction by
            scale = matrix[j * n + i];

            // Iterate over the remaining columns
            for(int k = i + 1; k < n; k++){
                matrix[j * n + k] -= matrix[i * n + k] * scale;
            }

            // Do direct assignment for trivial case (eliminate position)
            matrix[j * n + i] = 0;
        }
    }
}

// Pthread function for computing Gaussian Elimination
// Takes a pointer to a struct of args as an argument
void *ge_parallel(void *args){
    // Unpack Arguments to struct
    Args *local_args = (Args*)args;
    int tid = local_args->tid;
    int num_threads = local_args->num_threads;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    int *counter = local_args->counter;
    pthread_mutex_t *mtx = local_args->mtx;
    pthread_cond_t *cond = local_args->cond;
    
    // Keep track of rows for this thread
    int rows_per_thread = N / num_threads;
    int start_row = rows_per_thread * tid;

    for(int i = 0; i < N; i++){
        // Only one thread should do pivot calculations at a time
        if((i >= start_row && i) < (start_row + rows_per_thread)){
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
        
    }
}

// Initialize a matrix with random numbers
// Takes a matrix and its dimension as arguments
void init_matrix(float *matrix, int N){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            matrix[i * N + j] = rand(); 
        }
    }
}

// Prints a matrix
// Takes a matrix and its dimension as arguments
void print_matrix(float *matrix, int N){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            cout << setprecision(4) << matrix[i * N + j] << " ";

        }
        cout << endl;
    }
    cout << endl;
}

// Verifies the solution of Gaussian Elimination to the serial impl.
// Takes two matrices and a their dimensions as arguments
void verify_solution(float *matrix1, float *matrix2, int N){
    // Error can not exceed this bound
    float epsilon = 0.005;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            // Fail if error exceeds epsilon
            assert(abs(matrix1[i * N + j] - matrix2[i * N + j]) <= epsilon);
        }
    }
}

int main(){
    // Number of threads to launch
    int num_threads = 2;
    
    // Create array of thread, lock, and arguments
    pthread_t threads[num_threads];
    pthread_mutex_t mtx;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    Args thread_args[num_threads];

    // Number of elements in square matrix
    int N = 8;

    // Declare our problem matrices
    float *matrix;
    float *matrix_pthread;
    size_t bytes = N * N * sizeof(float);

    // Allocate space for our matrices
    matrix = (float*)malloc(bytes);
    matrix_pthread = (float*)malloc(bytes);
   
    // Create our progress counter for the threads
    int *counter = new int(num_threads);

    // Initialize matrix
    init_matrix(matrix, N);
    memcpy(matrix_pthread, matrix, bytes);
    
    // Launch threads
    for(int i = 0; i < num_threads; i++){
        thread_args[i].tid = i;
        thread_args[i].num_threads = num_threads;
        thread_args[i].matrix = matrix_pthread;
        thread_args[i].N = N;
        thread_args[i].mtx = &mtx;
        thread_args[i].counter = counter;
        thread_args[i].cond = &cond;

        pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]);
    }
    
    ge_serial(matrix, N);

    void *ret;
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], &ret);
    }
   
    //verify_solution(matrix, matrix_pthread, N);
    print_matrix(matrix_pthread, N);

    return 0;
}

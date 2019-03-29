// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

using namespace std;

struct Args {
    // Thread ID
    int tid;
    int num_threads;
    float *matrix;
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

int main(){
    // Number of threads to launch
    //int num_threads = 2;
    //pthread_t threads[num_threads];
    //Args thread_args[num_threads];
    
    // Number of elements in square matrix
    int N = 8;
    float *matrix;
    matrix = (float*)malloc(N * N * sizeof(float));
    
    // Initialize matrix
    init_matrix(matrix, N);
    print_matrix(matrix, N);
    ge_serial(matrix, N);
    print_matrix(matrix, N);
    /*
    // Launch threads
    for(int i = 0; i < num_threads; i++){
        pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]);
    }
    
    void *ret;
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], &ret);
    }
    */
    return 0;
}

// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <pthread.h>
#include <stdlib.h>
#include "../../common/common.h"
#include "utils.h"

int main(){
    // Number of threads to launch
    int num_threads = 4;

    // Dimensions of square matrix
    int N = 8;

    // Declare our problem matrices
    float *matrix;
    float *matrix_pthread;

    // Declare and initialize the size of the matrix
    size_t bytes = N * N * sizeof(float);

    // Allocate space for our matrices
    matrix = (float*)malloc(bytes);
    matrix_pthread = (float*)malloc(bytes);
   
    // Initialize a matrix and copy it
    init_matrix(matrix, N);
    memcpy(matrix_pthread, matrix, bytes);
    
    // Launch the threads via a helper function
    pthread_t *threads = launch_threads(num_threads, matrix_pthread, N);

    // Join threads via a helper function
    join_threads(num_threads, threads);

    // Call the serial version for our reference solution
    ge_serial(matrix, N);
 
    // Verify the solution
    verify_solution(matrix, matrix_pthread, N);

    return 0;
}

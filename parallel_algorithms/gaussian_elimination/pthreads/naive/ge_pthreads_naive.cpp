// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <stdlib.h>
#include "utils.h"

int main(){
    // Number of threads to launch
    int num_threads = 2;

    // Dimensions of square matrix
    int N = 2048;

    // Declare our problem matrices
    float *matrix;
    float *matrix_pthread;

    // Declare and initialize the size of the matrix
    size_t bytes = N * N * sizeof(float);

    // Allocate space for our matrices
    matrix = new float[N * N];
    matrix_pthread = new float[N * N];

    // Initialize a matrix and copy it
    init_matrix(matrix, N);
    memcpy(matrix_pthread, matrix, bytes);

    // Launch the threads via a helper function
    // Prints out time in seconds
    launch_threads(num_threads, matrix_pthread, N);

    // Create timers for our serial version
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;

    // Call the serial version for our reference solution
    start = high_resolution_clock::now();
    ge_serial(matrix, N);
    end = high_resolution_clock::now();
    
    // Cast timers as double to print
    duration<double> elapsed = duration_cast<duration<double>>(end - start);

    // Print out the elapsed time
    cout << "Elapsed time serial = " << elapsed.count() << " seconds" <<  endl;

    // Verify the solution
    verify_solution(matrix, matrix_pthread, N);
    
    // Free heap-allocated memory
    delete[] matrix;
    delete[] matrix_pthread;

    return 0;
}


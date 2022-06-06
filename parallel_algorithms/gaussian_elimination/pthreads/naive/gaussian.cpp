// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <cstdlib>
#include <memory>
#include "utils.h"

int main(){
    // Number of threads to launch
    const int num_threads = 8;

    // Dimensions of square matrix
    const int N = 2048;

    // Declare our problem matrices
    std::unique_ptr<float[]> matrix = std::make_unique<float[]>(N * N);
    std::unique_ptr<float[]> matrix_pthread = std::make_unique<float[]>(N * N);

    // Declare and initialize the size of the matrix
    size_t bytes = N * N * sizeof(float);

    // Initialize a matrix and copy it
    init_matrix(matrix.get(), N);
    memcpy(matrix_pthread.get(), matrix.get(), bytes);

    // Launch the threads via a helper function
    // Prints out time in seconds
    launch_threads(num_threads, matrix_pthread, N);

    // Create timers for our serial version
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;

    // Call the serial version for our reference solution
    start = high_resolution_clock::now();
    ge_serial(matrix.get(), N);
    end = high_resolution_clock::now();
    
    // Cast timers as double to print
    duration<double> elapsed = duration_cast<duration<double>>(end - start);

    // Print out the elapsed time
    cout << "Elapsed time serial = " << elapsed.count() << " seconds" <<  endl;

    // Verify the solution
    verify_solution(matrix.get(), matrix_pthread.get(), N);

    return 0;
}


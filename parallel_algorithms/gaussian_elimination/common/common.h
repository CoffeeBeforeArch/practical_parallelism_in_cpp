// This file contains common functions used by the different versions
// of the Gaussian Elimination algorithm
// By: Nick from CoffeeBeforeArch

#include <iostream>
#include <iomanip>
#include <cstring>
#include <assert.h>
#include <time.h>

using namespace std;

// Serial function for computing Gaussian Elimination
// Takes a pointer to a matrix and its dimension as arguments 
void ge_serial(float *matrix, int n){
    // Iterate over each row in the matrix
    float pivot;
    for(int i = 0; i < n - 1; i++){
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

    // Trivial case of last row with only one element
    matrix[(n - 1) * n + n - 1] = 1;
}

// Initialize a matrix with random numbers
// Takes a matrix and its dimension as arguments
void init_matrix(float *matrix, int N){
    srand(time(NULL));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            // Random number between 100 and -100
            matrix[i * N + j] = (float(rand())/float(RAND_MAX)) * (100 - -100) + -100; 
        }
    }
}

// Prints a matrix
// Takes a matrix and its dimension as arguments
void print_matrix(float *matrix, int N){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            cout << setprecision(3) << matrix[i * N + j] << "\t";

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


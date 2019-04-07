// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <stdlib.h>
#include <mpi.h>
#include "../../common/common.h"

int main(int argc, char *argv[]){
    // Declare a problem size
    int N = 8
    
    // Unique rank for this process
    int rank;
    
    // Total number of ranks
    int size;

    // Initializes the MPI execution environment
    MPI_Init(&argc, &argv);
    
    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number ranks in this communicator
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Calulate the number of rows basd on the number of ranks
    int num_rows = N / size; 

    // Declare our problem matrices
    float *matrix;

    if(rank == 0){
        // Allocate space for our matrix
        // Will be copied in messages and scattered to other processes
        matrix = new float[N * N];
        
        // Initialize the matrix
        init_matrix(matrix, N);
        
        // Send data to the other ranks
        for(int i = 1; i < size; i++){

        }
    }else{
        // Only allocate the data we need
        matrix = new float[num_rows * N];
    }

    // Free heap-allocated memory
    delete[] matrix;

    return 0;
}


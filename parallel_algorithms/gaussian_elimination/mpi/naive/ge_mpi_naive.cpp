// This program implements parallel gaussian elimination in C++ using
// Pthreads (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <stdlib.h>
#include <mpi.h>
#include "../../common/common.h"

int main(int argc, char *argv[]){
    // Declare a problem size
    int N = 8;
    
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

    /*
     * Distribute Work to Ranks:
     * Rank 0 needs to send the appropriate rows to each process
     * before they are able to proceed
     */
    // Declare our problem matrices
    float *matrix;
    float *row = new float[N];
    if(rank == 0){
        // Allocate space for our matrix
        // Will be copied in messages and scattered to other processes
        matrix = new float[N * N];
        
        // Initialize the matrix
        init_matrix(matrix, N);
        
        // Send data to the other ranks
        for(int i = 1; i < size; i++){
            MPI_Send(matrix + (N * i * num_rows), N * num_rows,
                    MPI_FLOAT, i, rank, MPI_COMM_WORLD);    
        }
    }else{
        // Receive the rows that this rank needs
        matrix = new float[N * num_rows];
        MPI_Recv(matrix, N * num_rows, MPI_FLOAT, 0,
                rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    /*
     * Gaussian Elimination:
     * One row normalizes the pivot row, then sends it to all
     * later ranks for elimination
     */
    // Allocate space for a single row to be sent to this rank
    for(int i = 0; i < N; i++){
        int which_rank = i / num_rows;
        int rank_start = rank * num_rows;
        int rank_end = rank * num_rows + num_rows;

        // Only one rank is normalizing a row at a time
        if((i > rank_start) && (i < rank_end)){
            int local_row = i % num_rows;
            for(int j = (rank + 1); j < size; j++){
                MPI_Send(matrix + (i * N), N, MPI_FLOAT, i, i,
                        MPI_COMM_WORLD);
            }
        }else if (i < rank_start){
            MPI_Recv(row, N, MPI_FLOAT, which_rank, rank,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);        
        }
    }

    // Free heap-allocated memory
    delete[] matrix;
    delete[] row;
    return 0;
}


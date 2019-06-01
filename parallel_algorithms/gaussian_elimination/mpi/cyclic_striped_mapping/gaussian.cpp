// This program implements parallel gaussian elimination in C++ using
// MPI and cyclic striped mapping (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <stdlib.h>
#include <mpi.h>
#include <cstring>
#include "../../common/common.h"

int main(int argc, char *argv[]){
    // Declare a problem size
    int N = 8;

    // Declate variables for timing
    double t_start;
    double t_end;
    double t_total;

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

    // Calulate the number of rows based on the number of ranks
    int num_rows = N / size; 

    /*
     * Distribute Work to Ranks:
     * Rank 0 needs to send the appropriate rows to each process
     * before they are able to proceed
     */
    // Declare our problem matrices
    // This work is duplicated just for code simplicity
    float *matrix = new float[N * N];
    init_matrix(matrix, N);
    
    // Declare our sub-matrix for each process
    float *sub_matrix = new float[N * num_rows];

    // Send a sub-matrix to each process (but striped)
    for(int i = 0; i < num_rows; i++){
        MPI_Scatter(matrix + (i * num_rows * N), N, MPI_FLOAT,
                sub_matrix + (i * N), N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    }

    /*
     * Gaussian Elimination:
     * One rank normalizes the pivot row, then sends it to all
     * later ranks for elimination
     */
    // Allocate space for a single row to be sent to this rank
    float *row = new float[N * num_rows];

    // Get start time
    if(rank == 0){
        t_start = MPI_Wtime();
    }

    // Barrier to track when calculations are done
    MPI_Barrier(MPI_COMM_WORLD);

    // Stop the time before the gather phase
    if(rank == 0){
        t_end = MPI_Wtime();
        t_total = t_end - t_start;
    }

    /*
     * Collect all Sub-Matrices
     * All sub-matrices are gathered using the gather function
     */

    for(int i = 0; i < num_rows; i++){
        // Get 1 rows from each rank
        // Offset by 1 row in the sub-matrix
        // Offset by 1 sub-matrix in the total matrix
        MPI_Gather(sub_matrix + (i * N), N, MPI_FLOAT,
                matrix + (i * N * num_rows), N, MPI_FLOAT, 0,
                MPI_COMM_WORLD);
    }

    MPI_Finalize();

    // Print the output and the time
    if(rank == 0){
        print_matrix(matrix, N);
        cout << t_total << " Seconds" << endl;
    }

    // Free heap-allocated memory
    delete[] matrix;
    delete[] sub_matrix;
    delete[] row;

    return 0;
}


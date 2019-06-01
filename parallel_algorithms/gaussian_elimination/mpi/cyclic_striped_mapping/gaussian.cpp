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
    if(size > 1){
        for(int i = 0; i < num_rows; i++){
            MPI_Scatter(matrix + (i * num_rows * N), N, MPI_FLOAT,
                sub_matrix + (i * N), N, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
    }else{
        memcpy(sub_matrix, matrix, N * N * sizeof(float));
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

    // Some variables to replace calculations with variables
    int local_row;
    int mapped_rank;

    // Value of pivot
    float pivot;

    // Iterate over all rows
    for(int i = 0; i < N; i++){
        // Caclulate the local row in the sub-matrix
        local_row = i % num_rows;
        mapped_rank = i % size;
        
        // If the row belongs to us
        if(mapped_rank == rank){
            // Get the pivot on the diagonal
            pivot = sub_matrix[local_row * N + i];

            // Divide the remaining elements in the row
            for(int j = i + 1; j < N; j++){
                sub_matrix[local_row * N + j] /= pivot;
            }

            // Use assignment instead of division for the diagonal
            sub_matrix[local_row * N + i] = 1;

            // Copy the row to the row-buffer
            memcpy(row, sub_matrix + local_row * N, N);

            // Send the normalized row to all other ranks
            MPI_Bcast(row, N, MPI_FLOAT, mapped_rank, MPI_COMM_WORLD);

        }else{
            // Receive the row from the broadcaster
            cout << "REC FROM " << rank << endl;
            MPI_Bcast(row, N, MPI_FLOAT, mapped_rank, MPI_COMM_WORLD);
        }
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
    if(size > 1){
        for(int i = 0; i < num_rows; i++){
            // Get 1 rows from each rank
            // Offset by 1 row in the sub-matrix
            // Offset by 1 sub-matrix in the total matrix
            MPI_Gather(sub_matrix + (i * N), N, MPI_FLOAT,
                matrix + (i * N * num_rows), N, MPI_FLOAT, 0,
                MPI_COMM_WORLD);
        }
    }else{
        memcpy(matrix, sub_matrix, N * N * sizeof(float));
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


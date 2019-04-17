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

    // Send a sub-matrix to each process
    MPI_Scatter(matrix, N * num_rows, MPI_FLOAT, sub_matrix,
            N * num_rows, MPI_FLOAT, 0, MPI_COMM_WORLD);

    /*
     * Gaussian Elimination:
     * One row normalizes the pivot row, then sends it to all
     * later ranks for elimination
     */
    // Allocate space for a single row to be sent to this rank
    float *row = new float[N * num_rows];

    // Get start time
    if(rank == 0){
        t_start = MPI_Wtime();
    }

    // Receivers go here
    for(int i = 0; i < (rank * num_rows); i++){
        // Wait for the preceeding ranks to forward us a row
        MPI_Bcast(row, N, MPI_FLOAT, i / num_rows, MPI_COMM_WORLD);

        // Eliminate from this element from all rows mapped to this
        // rank
        for(int j = 0; j < num_rows; j++){
            // Subtract from all other elements in the row
            for(int k = i + 1; k < N; k++){
                sub_matrix[j * N + k] -= sub_matrix[j * N + i] * row[k];
            }

            // Eliminate the element in the same column as the pivot row
            sub_matrix[j * N + i] = 0;
        }
    }

    // Senders go here
    int pivot_column;
    for(int i = 0; i < num_rows; i++){
        // Normalize this row to the pivot
        pivot_column = rank * num_rows + i;
        for(int j = pivot_column + 1; j < N; j++){
           sub_matrix[i * N + j] /= sub_matrix[i * N + pivot_column];
        }

        // Normalize trivial case
        sub_matrix[i * N + pivot_column] = 1;

        // Fill row to be sent
        memcpy(row, sub_matrix + (i * N), N * sizeof(float));

        // Broadcast the normalized row to everyone else;
        MPI_Bcast(row, N, MPI_FLOAT, rank, MPI_COMM_WORLD);

        // Update the rest of the rows for this rank
        for(int j = i + 1; j < num_rows; j++){
            for(int k = pivot_column + 1; k < N; k++){
                sub_matrix[j * N + k] -= sub_matrix[j * N + i] * row[k];
            }
            
            // Eliminate the trivial case
            sub_matrix[j * N + pivot_column] = 0;
        }
    }

    // Finished ranks must still wait with synchronous broadcast
    for(int i = rank * num_rows + 1; i < N; i++){
        MPI_Bcast(row, N, MPI_FLOAT, i / num_rows, MPI_COMM_WORLD);
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
    MPI_Gather(sub_matrix, N * num_rows, MPI_FLOAT, matrix,
            N * num_rows, MPI_FLOAT, 0, MPI_COMM_WORLD);


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


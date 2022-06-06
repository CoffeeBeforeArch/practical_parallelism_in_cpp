// This program implements parallel gaussian elimination in C++ using
// MPI and cyclic striped mapping (assumes square matrix)
// By: Nick from CoffeeBeforeArch

#include <mpi.h>
#include <cstring>
#include <memory>
#include "../../common/common.h"

int main(int argc, char *argv[]){
    // Declare a problem size
    const int N = 1024;

    // Declare variables for timing
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
    
    // Calculate the number of rows based on the number of ranks
    const int num_rows = N / size;

    /*
     * Distribute Work to Ranks:
     * Rank 0 needs to send the appropriate rows to each process
     * before they are able to proceed
     */
    // Declare our problem matrices
    // This work is duplicated just for code simplicity
    std::unique_ptr<float[]> matrix;
    if(rank == 0){
        // Only rank 0 needs space for the total solution
        matrix = std::make_unique<float[]>(N * N);

        // Initialize the matrix
        init_matrix(matrix.get(), N);
    }

    // Declare our sub-matrix for each process
    std::unique_ptr<float[]> sub_matrix = std::make_unique<float[]>(N * num_rows);

    // Cyclic stripe the rows to all the ranks
    if(size == 1){
        // All rows to the single rank
        memcpy(sub_matrix.get(), matrix.get(), N * N * sizeof(float));
    }else{
        // Scatter "num_rows" rows to "size" processes
        for(int i = 0; i < num_rows; i++){
            MPI_Scatter(&matrix[i * N * size], N, MPI_FLOAT,
                &sub_matrix[i * N], N, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
    }

    /*
     * Gaussian Elimination:
     * One rank normalizes the pivot row, then sends it to all
     * later ranks for elimination
     */
    // Allocate space for a single row to be sent to this rank
    std::unique_ptr<float[]> row = std::make_unique<float[]>(N);

    // Get start time
    if(rank == 0){
        t_start = MPI_Wtime();
    }
    
    // Local variables for code clarity
    float scale;

    // Iterate over all rows
    for(int i = 0; i < N; i++){
        // Which row in the sub-matrix are we accessing?
        const int local_row = i / size;
        // Which rank does this row belong to?
        const int which_rank = i % size;
        
        // Eliminate if the pivot belongs to this rank
        if(rank == which_rank){
            const float pivot = sub_matrix[local_row * N + i];
            
            // Divide the rest of the row by the pivot
            for(int j = i + 1; j < N; j++){
                sub_matrix[local_row * N + j] /= pivot;
            }

            // Use assignment for the trivial self-division
            sub_matrix[local_row * N + i] = 1;

            // Copy the row into our send buffer
            memcpy(row.get(), &sub_matrix[local_row * N], N * sizeof(float));

            // Broadcast this row to all the ranks
            MPI_Bcast(row.get(), N, MPI_FLOAT, which_rank, MPI_COMM_WORLD);

            // Eliminate for the other rows mapped to this rank
            for(int j = local_row + 1; j < num_rows; j++){
                scale = sub_matrix[j * N + i];

                // Subtract to eliminate pivot from later rows
                for(int k = i + 1; k < N; k++){
                    sub_matrix[j * N + k] -= scale * row[k];
                }

                // Use assignment for the trivial elimination
                sub_matrix[j * N + i] = 0;
            }
        }else{
            // Receive a row to use for elimination
            MPI_Bcast(row.get(), N, MPI_FLOAT, which_rank, MPI_COMM_WORLD);

            // Eliminate for all the rows mapped to this rank
            for(int j = local_row; j < num_rows; j++){
                if((which_rank < rank) || (j > local_row)){
                    scale = sub_matrix[j * N + i];

                    //Subtract to eliminate pivot from later rows
                    for(int k = i + 1; k < N; k++){
                        sub_matrix[j * N + k] -= scale * row[k];
                    }

                    // Use assignment for the trivial elimination
                    sub_matrix[j * N + i] = 0;
                }
            }
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
    if(size == 1){
        memcpy(matrix.get(), sub_matrix.get(), N * N * sizeof(float));
    }else{
        // Gather "size" rows at a time
        for(int i = 0; i < num_rows; i++){
            MPI_Gather(&sub_matrix[i * N], N, MPI_FLOAT,
                &matrix[i * size * N], N, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();

    // Print the output and the time
    if(rank == 0){
        //print_matrix(matrix, N);
        cout << t_total << " Seconds" << endl;
    }

    return 0;
}


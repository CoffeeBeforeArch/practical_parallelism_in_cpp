// This file contains utility functions for the pthread parallel
// Gaussian Elimination
// By: Nick from CoffeeBeforeArch

struct Args {
    // First row assigned to this thread
    int start_row;
    // One past the last row for this thread
    int end_row;
    // Matrix of floating point numbers
    float *matrix;
    // Dimensions of the square matrix
    int N;
    // Barrier to synchronize at
    pthread_barrier_t *barrier;
};

// Pthread function for computing Gaussian Elimination
// Takes a pointer to a struct of args as an argument
void *ge_parallel(void *args){
    // Cast void pointer to struct pointer
    Args *local_args = (Args*)args;

    // Unpack the arguments
    int start_row = local_args->start_row;
    int end_row = local_args->end_row;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    pthread_barrier_t *barrier = local_args->barrier;

    // Loop over all rows in the matrix
    for(int i = 0; i < N; i++){
        // Check if pivot row belongs to this thread
        if((i >= start_row) && (i < end_row)){
            // Normalize this row to the pivot
            float pivot = matrix[i * N + i];

            // Loop over remaining elements in the pivot row
            for(int j = i + 1; j < N; j++){
                matrix[i * N + j] /= pivot;
            }

            // Use assignment for trivial case
            matrix[i * N + i] = 1;
        }

        // All threads must wait for pivot before continuing
        pthread_barrier_wait(barrier);

        // Loop over the rest of the rows to eliminate the ith element
        for(int j = i + 1; j < end_row; j++){
            // Check if row belongs to this thread
            if((j >= start_row) && (j < end_row)){
                // Scale the subtraction by the ith element of this row
                float scale = matrix[j * N + i];

                // Subtract from each element of the row
                for(int l = i + 1; l < N; l++){
                    matrix[j * N + l] -= matrix[i * N + l] * scale; 
                }

                // Use assignment for trivial case
                matrix[j * N + i] = 0;
            }
        }
    }

    return 0;
}

// Helper function create thread 
pthread_t *launch_threads(int num_threads, float* matrix, int N){

    // Create array of thread objects we will launch
    pthread_t *threads = new pthread_t[num_threads];

    // Create a barrier and initialize it
    pthread_barrier_t *barrier = new pthread_barrier_t;
    pthread_barrier_init(barrier, NULL, num_threads);

    // Create an array of structs to pass to the threads
    Args *thread_args = new Args[num_threads];

    // Launch threads
    for(int i = 0; i < num_threads; i++){
        // Pack struct with its arguments
        thread_args[i].start_row = i * (N / num_threads);
        thread_args[i].end_row = i * (N / num_threads) + (N / num_threads);
        thread_args[i].matrix = matrix;
        thread_args[i].N = N;
        thread_args[i].barrier = barrier;
        
        // Launch the thread
        pthread_create(&threads[i], NULL, ge_parallel, (void*)&thread_args[i]);
    }

    return threads;
}

// Helper function to join threads
// Takes the number of threads, and pointer to array of threads as
// arguments
void join_threads(int num_threads, pthread_t *threads){
    void *ret;
    for(int i = 0; i < num_threads; i++){
        pthread_join(threads[i], &ret);
    }
}

// This file contains utility functions for the pthread parallel
// Gaussian Elimination
// By: Nick from CoffeeBeforeArch

struct Args {
    // Threaed ID
    int tid;
    // Number of threads launched
    int num_threads;
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
    int tid = local_args->tid;
    int num_threads = local_args->num_threads;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    pthread_barrier_t *barrier = local_args->barrier;

    // Loop over all rows in the matrix
    for(int i = 0; i < N; i++){
        // Check if pivot row belongs to this thread
        if((i % num_threads) == tid){
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
        for(int j = i + 1; j < N; j++){
            // Check if row belongs to this thread
            if((j % num_threads) == tid){
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
    
    // Number of rows to assign each thread
    int rows_per_thread = N / num_threads; 
    
    // Launch threads
    for(int i = 0; i < num_threads; i++){

        // Pack struct with its arguments
        thread_args[i].tid = i;
        thread_args[i].num_threads = num_threads;
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

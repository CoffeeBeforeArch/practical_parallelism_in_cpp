// This file contains utility functions for the pthread parallel
// Gaussian Elimination
// By: Nick from CoffeeBeforeArch

struct Args {
    // Number of threads in the program
    int num_threads;
    // First row assigned to this thread
    int start_row;
    // One past the last row for this thread
    int end_row;
    // Matrix of floating point numbers
    float *matrix;
    // Dimensions of the square matrix
    int N;
    // Counter used to track arrival of threads
    int *counter;
    // Mutex lock to ensure counter updated atomically
    pthread_mutex_t *mtx;
    // Condition for threads to wait on
    pthread_cond_t *cond;
};

// This function atomically decrements a counter for waiting threads.
// All threads must arrive at this location before the counter is reset,
// then all threads are signaled to continue
// Takes a mutex, outstanding thread count, total number of threads, and
// condition variable as arguments
void barrier(pthread_mutex_t *mtx, int *counter, int num_threads, 
        pthread_cond_t *cond){
        // Last thread resets the counter in the critical section
        pthread_mutex_lock(mtx);

        // Decrement threads as they arrive
        *counter -= 1;

        // We are the last thread
        if(*counter == 0){
            *counter = num_threads;
            // Broadcast the condition to waiting threads
            pthread_cond_broadcast(cond);
        }else{
            // Go to sleep until condition is broadcasted
            pthread_cond_wait(cond, mtx);
        }

        // Unlock at the end of the critical section
        pthread_mutex_unlock(mtx);
}

// Pthread function for computing Gaussian Elimination
// Takes a pointer to a struct of args as an argument
void *ge_parallel(void *args){
    // Cast void pointer to struct pointer
    Args *local_args = (Args*)args;

    // Unpack the arguments
    int num_threads = local_args->num_threads;
    int start_row = local_args->start_row;
    int end_row = local_args->end_row;
    float *matrix = local_args->matrix;
    int N = local_args->N;
    int *counter = local_args->counter;
    pthread_mutex_t *mtx = local_args->mtx;
    pthread_cond_t *cond = local_args->cond;

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
        barrier(mtx, counter, num_threads, cond);

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

    // Create a mutex lock and initialize it
    pthread_mutex_t *mtx = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);

    // Create a condition and initialize it
    pthread_cond_t *cond = new pthread_cond_t(PTHREAD_COND_INITIALIZER);

    // Create an array of structs to pass to the threads
    Args *thread_args = new Args[num_threads];

    // Create our progress counter for the threads
    int *counter = new int(num_threads);

    // Launch threads
    for(int i = 0; i < num_threads; i++){
        // Pack struct with its arguments
        thread_args[i].start_row = i * (N / num_threads);
        thread_args[i].end_row = i * (N / num_threads) + (N / num_threads);
        thread_args[i].num_threads = num_threads;
        thread_args[i].matrix = matrix;
        thread_args[i].N = N;
        thread_args[i].counter = counter;
        thread_args[i].mtx = mtx;
        thread_args[i].cond = cond;
        
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

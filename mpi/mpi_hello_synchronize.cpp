// This program shows off the basics of using MPI with C++
// with synchronized output
// By: Nick from CoffeeBeforeArch

#include <stdio.h>
#include <mpi.h>

using namespace std;

int main(int argc, char *argv[]){
    // Unique rank is assigned to each process in a communicator
    int rank;

    // Total number of ranks
    int size;

    // The machine we are on
    char name[80];

    // Length of the machine name
    int length;

    // Initializes the MPI execution environment
    MPI_Init(&argc, &argv);
    
    // Get this process' rank (process within a communicator)
    // MPI_COMM_WORLD is the default communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number ranks in this communicator
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Gets the name of the processor
    // Implementation specific (may be gethostname, uname, or sysinfo)
    MPI_Get_processor_name(name, &length);

    // Pack these values together into a string
    int buffer_len = 150;
    char buffer[buffer_len];
    sprintf(buffer, "Hello, MPI! Rank: %d Total: %d Machine %s",
            rank, size, name);

    // Synchronize so we can remove interleaved output
    if(rank == 0){
        // Always print from rank 0
        printf("%s\n", buffer);
        for(int i = 1; i < size; i++){
            // Takes buffer, size, type, source, tag, communicator, and status
            MPI_Recv(buffer, buffer_len, MPI_CHAR, i, i,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Print our received message
            printf("%s\n", buffer);
        }
    }else{
        // If not rank zero, send your message to be printed
        MPI_Send(buffer, buffer_len, MPI_CHAR, 0, rank, MPI_COMM_WORLD);    
    }
    
    // Terminate MPI execution environment
    MPI_Finalize();
}

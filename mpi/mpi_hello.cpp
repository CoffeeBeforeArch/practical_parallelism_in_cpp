// This program shows off the basics of using MPI with C++
// By: Nick from CoffeeBeforeArch

#include <iostream>
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

    // Print out for each rank
    cout << "Hello, MPI! Rank: " << rank << " Total: " << size <<
        " Machine: " << name << endl;

    // Terminate MPI execution environment
    MPI_Finalize();

    return 0;
}

#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include "coordinator.h"
#include "participant.h"
#include "../args/arguments.h"

#define FILELEN 256


int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);
    // Find out rank, size
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // We are assuming at least 2 processes for this task
    if (world_size < 2)
    {
        fprintf(stderr, "World size must be greater than 1 for %s\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    if (world_rank == 0)
    {
        Arguments args;
        if(!get_args(&args, argc, argv)){
            fprintf(stderr, "Argument parsing failed, aborting\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        main_routine(world_size, args.infile, args.outfile, args.iterations, args.clusters);
    }
    else if (world_rank < world_size)
    {
        worker_routine(world_rank);
    }
    MPI_Finalize();
}

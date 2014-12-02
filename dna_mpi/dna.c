#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include "coordinator_dna.h"
#include "participant_dna.h"

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
        char infile[FILELEN];
        char outfile[FILELEN];
        int iterations = 1;
        int clusters = 2;
        int c;

        opterr = 0;
        while ((c = getopt (argc, argv, "o:d:i:c:")) != -1){
            switch (c)
            {
            case 'd':
                printf("infile %s\n", optarg);
                strncpy(infile, optarg, FILELEN);
                break;
            case 'o':
                printf("outfile %s\n", optarg);
                strncpy(outfile, optarg, FILELEN);
                break;
            case 'i':
                iterations = atoi(optarg);
                break;
            case 'c':
                clusters = atoi(optarg);
                break;
            default:
                abort ();
            }
        }
        printf("%s %s\n", infile, outfile);
        main_routine(world_size, infile, outfile, iterations, clusters);
    }
    else if (world_rank < world_size)
    {
        worker_routine(world_rank);
    }
    MPI_Finalize();
}

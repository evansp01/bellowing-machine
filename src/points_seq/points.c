#include <time.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "coordinator.h"
#include "../args/arguments.h"

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    Arguments args;
    if (!get_args(&args, argc, argv))
    {
        fprintf(stderr, "Argument parsing failed, aborting\n");
        return 1;
    }
    main_routine(args.infile, args.outfile, args.iterations, args.clusters);
}

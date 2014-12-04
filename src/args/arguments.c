#include <getopt.h>
#include "arguments.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void usage(char *name){
	printf("%s -d DATASET -o OUTFILE -i ITERATIONS -c CLUSTERS\n", name);
	printf(" -d DATASET      The dataset to run k-means on\n");
	printf(" -o OUTFILE      The file to write the output of k-means to\n");
	printf(" -i ITERATIONS   The number of iterations of k-means to run\n");
	printf(" -c CLUSTERS     The number of clusters to attempt to find\n");
}

bool get_args(Arguments *args, int argc, char **argv)
{
    opterr = 0;
    char c;
    bool ifspec = false, ofspec = false, itspec = false, clspec = false;
    while ((c = getopt (argc, argv, "o:d:i:c:")) != -1)
    {
        switch (c)
        {
        case 'd':
        	ifspec = true;
            printf("infile %s\n", optarg);
            strncpy(args->infile, optarg, FILELEN);
            break;
        case 'o':
        	ofspec = true;
            printf("outfile %s\n", optarg);
            strncpy(args->outfile, optarg, FILELEN);
            break;
        case 'i':
        	itspec = true;
            args->iterations = atoi(optarg);
            break;
        case 'c':
        	clspec = true;
            args->clusters = atoi(optarg);
            break;
        default:
            printf("Unexpected input flag %c\n\n", c);
            usage(argv[0]);
            return false;
        }
    }
    if(ifspec && ofspec && itspec && clspec){
    	return true;
    }
    printf("The options -d -o -i -c must all be specified\n");
    usage(argv[0]);
}
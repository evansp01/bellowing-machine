#ifndef ARGUMENTS_
#define ARGUMENTS_

#define FILELEN 256
#include <stdbool.h>

struct arguments {
	char infile[FILELEN];
	char outfile[FILELEN];
	int iterations;
	int clusters;
} typedef Arguments;

bool get_args(Arguments *args, int argc, char **argv);

#endif
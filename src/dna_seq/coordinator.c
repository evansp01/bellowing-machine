#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "../debug/debug.h"

static int dna_dist(char *mean, char *point, int len)
{
    int similar = 0;
    for (int i = 0; i < len; i++)
    {
        if (mean[i] == point[i])
        {
            similar++;
        }
    }
    return similar;
}

static int dna_most_similar(int k, int len, char *means, char *point)
{
    int max_similar = -1;
    int min_index = -1;
    for (int i = 0; i < k; i++)
    {
        char *mean = &means[i * len];
        int similar = dna_dist(mean, point, len);
        if (similar > max_similar)
        {
            min_index = i;
            max_similar = similar;
        }
    }
    return min_index;
}

int letter_to_index(char c)
{
    switch (c)
    {
    case 'T':
        return 0;
    case 'C':
        return 1;
    case 'A':
        return 2;
    case 'G':
        return 3;
    }
}


void calc_sums_counts(char *points, char *means, int *mean_sums,
                      int *mean_counts, int k, int len, int n)
{
    memset(mean_sums, 0, len * 4 * k * sizeof(int));
    memset(mean_counts, 0, k * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        char *point = &points[len * i];
        int mean_index = dna_most_similar(k, len, means, point);
        for (int j = 0; j < len; j++)
        {
            int charindex = letter_to_index(point[j]);
            mean_sums[len * 4 * mean_index + 4*j + charindex] += 1;
        }
        mean_counts[mean_index] += 1;
    }
}



//Initialize the means to random values
static char *init_means(int n, int k, int len, char *points)
{
    srand(time(NULL));
    int in, ik;
    int *vektor = (int *) malloc(k * sizeof(int));
    ik = 0;
    for (in = 0; in < n && ik < k; ++in)
    {
        int rn = n - in;
        int rk = k - ik;
        if (rand() % rn < rk)
            /* Take it */
            vektor[ik++] = in;
    }
    char *means = (char *) malloc((len * k + 1) * sizeof(char));
    for (int i = 0; i < k; i++)
    {
        strncpy(&means[i * len], &points[len * vektor[i]], len);
        debugf ("Mean %d = %.*s\n", i, len, &points[(len)*i]);
    }
    free(vektor);
    debugf("\n");
    return means;
}

//Read the input to k-means from a file
static char *read_input(char *filename, int *len, int *n)
{
    debugf("Opening file %s\n", filename);
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        return NULL;
    }
    fscanf(f, "%d %d\n", len, n);
    debugf("Read dataset file %s with len=%d n=%d\n", filename, *len, *n);
    char *points = (char *) malloc((*len * (*n) + 1) * sizeof(char));
    char format[24];
    sprintf(format, " %%%ds", *len);
    for (int i = 0; i < *n; i++)
    {
        fscanf(f, format, &points[(*len)*i]);
    }
    fclose(f);
    return points;
}


static void recalculate_means(int *mean_sums, char *means, int k, int len)
{
    for (int i = 0; i < k * len; i++)
    {
        int *character = &mean_sums[i * 4];

        int max = 0;
        int max_index = -1;
        //printf("Character %d\n",i);
        for (int j = 0; j < 4; j++)
        {
            //printf("%d ",character[j]);
            if (character[j] > max)
            {
                max = character[j];
                max_index = j;
            }
        }
        //printf("\n");
        char c;
        switch (max_index)
        {
        case 0:
            c = 'T';
            break;
        case 1:
            c = 'C';
            break;
        case 2:
            c = 'A';
            break;
        case 3:
            c = 'G';
            break;
        }
        means[i] = c;
    }
}

//Run some number of iterations of k-means
static void run_iterations(int n, int k, int len, int iterations, char *means, char *points)
{
    debugf("entered iterations\n");
    int *mean_sums = (int *) malloc(k * len * 4 * sizeof(int));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    for (int i = 0; i < iterations; i++)
    {
        calc_sums_counts(points, means, mean_sums, mean_counts, k, len, n);
        recalculate_means(mean_sums, means, k , len);
    }
    free(mean_counts);
    free(mean_sums);
}

//Loop through every process, and inform them that they are all done


static void output_to_file(char *outfile, char *means, int k, int len)
{

    FILE *of = fopen(outfile, "w");
    if (of == NULL)
    {
        fprintf(stderr, "Could not open output file\n");
        return;
    }
    for (int i = 0; i < k; i++)
    {
        fprintf (of, "Mean %d = %.*s\n", i, len, &means[(len)*i]);
    }

}

void main_routine(char *infile, char *outfile, int iterations, int k)
{
    int n, len;
    char *points = read_input(infile, &len, &n);
    if (points == NULL)
    {
        fprintf(stderr, "Could not open input file\n");
        return;
    }
    char *means = init_means(n, k, len, points);
    debugf("Finished initialization\n");

    run_iterations(n, k, len, iterations, means, points);
    output_to_file(outfile, means, k, len);
    free(means);
    free(points);

}
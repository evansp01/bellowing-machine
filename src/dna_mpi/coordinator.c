#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "constants.h"
#include "../debug/debug.h"



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

//Split up the input points across the workers
static void split_points(int n, int k, int len, int world_size, char *points)
{
    int workers = world_size - 1;
    int elems_left = n;
    int workers_left = workers;
    int *starts = (int *) malloc(world_size * sizeof(int));
    starts[0] = 0;
    for (int i = 1; i < workers; i++)
    {
        int alloc_elems = (elems_left + workers_left - 1) / workers_left;
        elems_left -= alloc_elems;
        workers_left -= 1;
        starts[i] = starts[i - 1] + alloc_elems;
    }
    starts[workers] = n;
    for (int t = 1; t < world_size; t++)
    {
        int start = starts[t - 1];
        int end = starts[t];
        int num_points = end - start;
        MPI_Send(&num_points, 1, MPI_INT, t, SEND_N, MPI_COMM_WORLD);
        MPI_Send(&k, 1, MPI_INT, t, SEND_K, MPI_COMM_WORLD);
        MPI_Send(&len, 1, MPI_INT, t, SEND_LEN, MPI_COMM_WORLD);
        MPI_Send(&points[len * start], len * num_points, MPI_CHAR, t, SEND_POINTS, MPI_COMM_WORLD);
    }
    free(starts);
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
        for (int j = 0; j < 4; j++)
        {
            if (character[j] > max)
            {
                max = character[j];
                max_index = j;
            }
        }
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
static double run_iterations(int n, int k, int len, int world_size,
                             int iterations, char *means)
{
    debugf("entered iterations\n");
    double wait_time = 0, temp;
    int *mean_sums = (int *) malloc(k * len * 4 * sizeof(int));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    int *sums_acc = (int *) malloc(k * len * 4 * sizeof(int));
    int *count_acc = (int *) malloc(k * sizeof(int));
    for (int i = 0; i < iterations; i++)
    {
        int cont = 1;
        for (int t = 1; t < world_size; t++)
        {
            MPI_Send(&cont, 1, MPI_INT, t, SEND_CONTINUE, MPI_COMM_WORLD);
            MPI_Send(means, len * k, MPI_CHAR, t, SEND_MEANS, MPI_COMM_WORLD);
        }
        memset(sums_acc, 0, k * len * 4 * sizeof(char));
        memset(count_acc, 0, k * sizeof(int));
        for (int t = 1; t < world_size; t++)
        {
            temp = MPI_Wtime();
            MPI_Recv(mean_sums, len * 4 * k, MPI_INT, t, REPLY_MEANS,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(mean_counts, k, MPI_INT, t, REPLY_COUNTS,
                     MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            wait_time += MPI_Wtime() - temp;
            for (int j = 0; j < k; j++)
            {
                count_acc[j] += mean_counts[j];
            }
            for (int j = 0; j < k * len * 4; j++)
            {
                sums_acc[j] += mean_sums[j];
            }
        }
        recalculate_means(sums_acc, means, k , len);
    }
    free(mean_counts);
    free(mean_sums);
    free(sums_acc);
    free(count_acc);
    return wait_time;
}

//Loop through every process, and inform them that they are all done
static void notify_done(int world_size)
{
    int cont = 0;
    for (int t = 1; t < world_size; t++)
    {
        MPI_Send(&cont, 1, MPI_INT, t, SEND_CONTINUE, MPI_COMM_WORLD);
    }
}

static void write_time(FILE *f, int num, double total, double wait)
{
    fprintf(f, "Process %d:: Runtime: %lf Waittime: %lf\n",
            num, total, wait);
}


static void output_to_file(char *outfile, char *means, int k, int len,
                           double total, double wait, int world)
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

    //get and write the timings
    fprintf(of, "--------\n");
    write_time(of, 0, total, wait);
    for (int t = 1; t < world; t++)
    {
        MPI_Recv(&total, 1, MPI_DOUBLE, t, TIME_TOTL,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&wait, 1, MPI_DOUBLE, t, TIME_WAIT,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        write_time(of, t, total, wait);
    }

}

void main_routine(int world_size, char *infile, char *outfile, int iterations, int k)
{
    int n, len;
    double total_time, start_time, wait_time;

    start_time = MPI_Wtime();
    char *points = read_input(infile, &len, &n);
    if (points == NULL)
    {
        fprintf(stderr, "Could not open input file\n");
        MPI_Abort(MPI_COMM_WORLD, 0);
        return;
    }
    char *means = init_means(n, k, len, points);
    split_points(n, k, len, world_size, points);
    debugf("Finished initialization\n");

    wait_time = run_iterations(n, k, len, world_size, iterations, means);
    notify_done(world_size);
    total_time = MPI_Wtime() - start_time;
    output_to_file(outfile, means, k, len, total_time, wait_time, world_size);
    free(means);
    free(points);

}
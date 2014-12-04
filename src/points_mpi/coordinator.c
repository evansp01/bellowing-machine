#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"

//Initialize the means to random values
static double *init_means(int n, int k, double *points)
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
    double *means = (double *) malloc(DIM * k * sizeof(double));
    for (int i = 0; i < k; i++)
    {
        means[DIM * i] = points[DIM * vektor[i]];
        means[DIM * i + 1] = points[DIM * vektor[i] + 1];
        printf("k%d = (%f,%f)  ", i, means[DIM * i], means[DIM * i + 1]);
    }
    free(vektor);
    printf("\n");
    return means;
}

//Split up the input points across the workers
static void split_points(int n, int k, int world_size, double *points)
{
    printf("splitting into %d partitions\n", k-1);
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
    printf("starts: ");
    for (int i = 0; i < workers + 1; i++)
    {
        printf("%d ", starts[i]);
    }
    printf("\n");
    for (int t = 1; t < world_size; t++)
    {
        int start = starts[t - 1];
        int end = starts[t];
        int num_points = end - start;
        MPI_Send(&num_points, 1, MPI_INT, t, SEND_N, MPI_COMM_WORLD);
        MPI_Send(&k, 1, MPI_INT, t, SEND_K, MPI_COMM_WORLD);
        MPI_Send(&points[DIM * start], DIM * num_points, MPI_DOUBLE, t, SEND_POINTS, MPI_COMM_WORLD);
    }
    free(starts);
}

//Read the input to k-means from a file
static double *read_input(char *filename, int *n)
{
    printf("Opening file %s\n", filename);
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        MPI_Abort(MPI_COMM_WORLD, 0);
        return NULL;
    }
    fscanf(f, " %d", n);
    printf("Read dataset file %s with n=%d\n", filename, *n);
    double *points = (double *) malloc(DIM * (*n) * sizeof(double));
    for (int i = 0; i < *n; i++)
    {
        fscanf(f, " %lf %lf", &points[DIM * i], &points[DIM * i + 1]);
        //printf("Point %d = (%f, %f)\n", i, points[DIM * i], points[DIM * i + 1]);
    }
    fclose(f);
    return points;
}

//Run some number of iterations of k-means
static void run_iterations(int n, int k, int world_size, int iterations, double *means)
{
    double *mean_sums = (double *) malloc(k * DIM * sizeof(double));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    double *sums_acc = (double *) malloc(k * DIM * sizeof(double));
    int *count_acc = (int *) malloc(k * sizeof(int));
    printf("Running %d iterations", iterations);
    for (int i = 0; i < iterations; i++)
    {
        int cont = 1;
        for (int t = 1; t < world_size; t++)
        {
            MPI_Send(&cont, 1, MPI_INT, t, SEND_CONTINUE, MPI_COMM_WORLD);
            MPI_Send(means, DIM * k, MPI_DOUBLE, t, SEND_MEANS, MPI_COMM_WORLD);
        }
        memset(sums_acc, 0, k * DIM * sizeof(double));
        memset(count_acc, 0, k * sizeof(int));
        for (int t = 1; t < world_size; t++)
        {
            MPI_Recv(mean_sums, DIM * k, MPI_DOUBLE, t, REPLY_MEANS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(mean_counts, k, MPI_INT, t, REPLY_COUNTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int j = 0; j < k; j++)
            {
                count_acc[j] += mean_counts[j];
                sums_acc[DIM * j] += mean_sums[DIM * j];
                sums_acc[DIM * j + 1] += mean_sums[DIM * j + 1];
            }
        }
        for (int j = 0; j < k; j++)
        {
            means[DIM * j] = sums_acc[DIM * j] / count_acc[j];
            means[DIM * j + 1] = sums_acc[DIM * j + 1] / count_acc[j];
        }
    }
    free(mean_counts);
    free(mean_sums);
    free(sums_acc);
    free(count_acc);
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

void main_routine(int world_size, char *infile, char *outfile, int iterations, int k)
{
    //seed random number generator

    int n;
    printf("%s\n", infile);
    double *points = read_input(infile, &n);
    if (points == NULL)
    {
        //should tell things to abort
        return;
    }
    double *means = init_means(n, k, points);
    split_points(n, k, world_size, points);
    printf("Finished initialization\n");

    run_iterations(n, k, world_size, iterations, means);
    notify_done(world_size);

    FILE *of = fopen(outfile, "w");
    if (of == NULL)
    {
        printf("Could not open output file\n");
        //perhaps we should do something
        return;
    }
    for (int i = 0; i < k; i++)
    {
        fprintf(of, "The mean %d is at (%f,%f)\n", i, means[DIM * i], means[DIM * i + 1]);
    }
    free(means);
    free(points);
}
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../debug/debug.h"

#define DIM 2

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
        debugf("k%d = (%f,%f)  ", i, means[DIM * i], means[DIM * i + 1]);
    }
    free(vektor);
    debugf("\n");
    return means;
}


static double sq_dist(double x1, double y1, double x2, double y2)
{
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

static int min_sq_dist(int k, double *means, double px, double py)
{
    double min = -1;
    int min_index = -1;
    for (int i = 0; i < k; i++)
    {
        double mx = means[DIM * i];
        double my = means[DIM * i + 1];
        double dist = sq_dist(mx, my, px, py);
        if (dist < min || min < 0)
        {
            min_index = i;
            min = dist;
        }
    }
    return min_index;
}

void calc_sums_counts(int k, int n, double *points, double *means,
                      double *mean_sums, int *mean_counts)
{
    memset(mean_sums, 0, DIM * k * sizeof(double));
    memset(mean_counts, 0, k * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        double px = points[DIM * i];
        double py = points[DIM * i + 1];
        int mean_index = min_sq_dist(k, means, px, py);
        mean_sums[DIM * mean_index] += px;
        mean_sums[DIM * mean_index + 1] += py;
        mean_counts[mean_index] += 1;
    }
}

//Read the input to k-means from a file
static double *read_input(char *filename, int *n)
{
    debugf("Opening file %s\n", filename);
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        return NULL;
    }
    fscanf(f, " %d", n);
    debugf("Read dataset file %s with n=%d\n", filename, *n);
    double *points = (double *) malloc(DIM * (*n) * sizeof(double));
    for (int i = 0; i < *n; i++)
    {
        fscanf(f, " %lf %lf", &points[DIM * i], &points[DIM * i + 1]);
    }
    fclose(f);
    return points;
}

//Run some number of iterations of k-means
static void run_iterations(int n, int k, int iterations, double *means, double *points)
{
    double wait_time = 0, temp;
    double *mean_sums = (double *) malloc(k * DIM * sizeof(double));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    debugf("Running %d iterations", iterations);
    for (int i = 0; i < iterations; i++)
    {
        calc_sums_counts(k, n, points, means, mean_sums, mean_counts);
        for (int j = 0; j < k; j++)
        {
            means[DIM * j] = mean_sums[DIM * j] / mean_counts[j];
            means[DIM * j + 1] = mean_sums[DIM * j + 1] / mean_counts[j];
        }
    }
    free(mean_counts);
    free(mean_sums);
}

//Write the mean value to a file
static void write_mean(FILE *f, int num, double x, double y)
{
    fprintf(f, "The mean %d is at (%lf,%lf)\n", num, x, y);
}

//write the time to a file
static void write_time(FILE *f, int num, double total, double wait)
{
    fprintf(f, "Runtime: %lf\n", total);
}

static void output_to_file(char *outfile, double *means, int k)
{
    FILE *of = fopen(outfile, "w");
    if (of == NULL)
    {
        fprintf(stderr, "Could not open output file\n");
        return;
    }
    for (int i = 0; i < k; i++)
    {
        write_mean(of, i, means[DIM * i], means[DIM * i + 1]);
    }
}

void main_routine(char *infile, char *outfile, int iterations, int k)
{
    int n;
    double total_time, start_time, wait_time;

    double *points = read_input(infile, &n);
    if (points == NULL)
    {
        fprintf(stderr, "Could not open output file\n");
        return;
    }
    double *means = init_means(n, k, points);
    run_iterations(n, k, iterations, means, points);
    //write output to file
    output_to_file(outfile, means,  k);
    free(means);
    free(points);
}
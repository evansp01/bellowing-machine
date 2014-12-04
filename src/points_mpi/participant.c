#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include "constants.h"

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

void worker_routine(int rank)
{
    int n, k;
    MPI_Recv(&n, 1, MPI_INT, MASTER, SEND_N, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&k, 1, MPI_INT, MASTER, SEND_K, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Worker %d recieved instructions that dataset contains %d points and will have %d means\n", rank, n, k);
    double *points = (double *) malloc(DIM * n * sizeof(double));
    double *means = (double *) malloc(DIM * k * sizeof(double));
    double *mean_sums = (double *) malloc(k * DIM * sizeof(double));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    MPI_Recv(points, DIM * n, MPI_DOUBLE, MASTER, SEND_POINTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    while (true)
    {
        int cont;
        MPI_Recv(&cont, 1, MPI_INT, MASTER, SEND_CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (!cont)
        {
            printf("Process %d recieved abort signal\n", rank);
            break;
        }
        MPI_Recv(means,  DIM * k, MPI_DOUBLE, MASTER, SEND_MEANS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        calc_sums_counts(k, n, points, means, mean_sums, mean_counts);
        MPI_Send(mean_sums, DIM * k, MPI_DOUBLE, MASTER, REPLY_MEANS, MPI_COMM_WORLD);
        MPI_Send(mean_counts, k, MPI_INT, MASTER, REPLY_COUNTS, MPI_COMM_WORLD);
    }
    free(mean_sums);
    free(mean_counts);
    free(points);
    free(means);
}
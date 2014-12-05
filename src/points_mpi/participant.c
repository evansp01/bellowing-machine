#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
#include "../debug/debug.h"

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

double get_info(int *n, int *k)
{
    double  temp = MPI_Wtime();
    MPI_Recv(n, 1, MPI_INT, MASTER, SEND_N, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(k, 1, MPI_INT, MASTER, SEND_K, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return  MPI_Wtime() - temp;
}

double get_points(double *points, int n)
{
    double  temp = MPI_Wtime();
    MPI_Recv(points, DIM * n, MPI_DOUBLE, MASTER, SEND_POINTS,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

double get_cont(int *cont)
{
    double temp = MPI_Wtime();
    MPI_Recv(cont, 1, MPI_INT, MASTER, SEND_CONTINUE,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

double get_means(double *means, int k)
{
    double temp = MPI_Wtime();
    MPI_Recv(means,  DIM * k, MPI_DOUBLE, MASTER, SEND_MEANS,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

void worker_routine(int rank)
{
    double start_time, total_time, wait_time = 0, temp;
    int n, k;
    start_time = MPI_Wtime();
    wait_time = get_info(&n, &k);
    debugf("Worker %d recieved instructions that dataset contains %d \
           points and will have %d means\n", rank, n, k);
    double *points = (double *) malloc(DIM * n * sizeof(double));
    double *means = (double *) malloc(DIM * k * sizeof(double));
    double *mean_sums = (double *) malloc(k * DIM * sizeof(double));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    wait_time += get_points(points, n);
    while (true)
    {
        int cont;
        wait_time += get_cont(&cont);
        if (!cont)
        {
            debugf("Process %d recieved abort signal\n", rank);
            break;
        }
        wait_time += get_means(means, k);
        calc_sums_counts(k, n, points, means, mean_sums, mean_counts);
        MPI_Send(mean_sums, DIM * k, MPI_DOUBLE, MASTER,
                 REPLY_MEANS, MPI_COMM_WORLD);
        MPI_Send(mean_counts, k, MPI_INT, MASTER, REPLY_COUNTS, MPI_COMM_WORLD);
    }
    free(mean_sums);
    free(mean_counts);
    free(points);
    free(means);
    total_time = MPI_Wtime() - start_time;
    MPI_Send(&total_time, 1, MPI_DOUBLE, MASTER, TIME_TOTL, MPI_COMM_WORLD);
    MPI_Send(&wait_time, 1, MPI_DOUBLE, MASTER, TIME_WAIT, MPI_COMM_WORLD);
}
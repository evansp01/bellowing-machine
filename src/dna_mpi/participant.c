#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "constants.h"
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
    int max_similar = 0;
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

double get_info(int *n, int *k, int *len)
{
    double temp = MPI_Wtime();
    MPI_Recv(n, 1, MPI_INT, MASTER, SEND_N,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(k, 1, MPI_INT, MASTER, SEND_K,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(len, 1, MPI_INT, MASTER, SEND_LEN,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    debugf("Worker %d received instructions that dataset contains %d \
        points of length %d and will have %d means\n", rank, n, len, k);
    return MPI_Wtime() - temp;
}

double get_continue(int *cont)
{
    double temp = MPI_Wtime();
    MPI_Recv(cont, 1, MPI_INT, MASTER, SEND_CONTINUE,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

double get_points(char *points, int len, int n)
{
    double temp = MPI_Wtime();
    MPI_Recv(points, len * n, MPI_CHAR, MASTER, SEND_POINTS,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

double get_means(char *means, int len, int k)
{
    double temp = MPI_Wtime();
    MPI_Recv(means,  len * k, MPI_CHAR, MASTER, SEND_MEANS,
             MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return MPI_Wtime() - temp;
}

void calc_sums_counts(char *points, char *means, int *mean_sums,
                      int *mean_counts, int k, int len, int n)
{
    memset(mean_sums, 0, len * 4 * k * sizeof(char));
    memset(mean_counts, 0, k * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        char *point = &points[len * i];
        int mean_index = dna_most_similar(k, len, means, point);
        for (int j = 0; j < len; j++)
        {
            int charindex = letter_to_index(point[j]);
            mean_sums[len * 4 * mean_index + charindex] += 1;
        }
        mean_counts[mean_index] += 1;
    }
}

void worker_routine(int rank)
{
    double start_time, total_time, wait_time;
    int n, k, len;
    start_time = MPI_Wtime();
    wait_time = get_info(&n, &k, &len);
    char *points = (char *) malloc(len * n * sizeof(char));
    char *means = (char *) malloc(len * k * sizeof(char));
    int *mean_sums = (int *) malloc(len * k * 4 * sizeof(int));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    wait_time += get_points(points, len, n);
    while (true)
    {
        int cont;
        wait_time += get_continue(&cont);
        if (!cont)
        {
            debugf("Process %d recieved abort signal\n", rank);
            break;
        }
        wait_time += get_means(means, len, k);
        calc_sums_counts(points, means, mean_sums, mean_counts, k, len, n);
        MPI_Send(mean_sums, 4 * len * k, MPI_INT, MASTER,
                 REPLY_MEANS, MPI_COMM_WORLD);
        MPI_Send(mean_counts, k, MPI_INT, MASTER,
                 REPLY_COUNTS, MPI_COMM_WORLD);
    }
    free(mean_sums);
    free(mean_counts);
    free(points);
    free(means);
    total_time = MPI_Wtime() - start_time;
    MPI_Send(&total_time, 1, MPI_DOUBLE, MASTER, TIME_TOTL, MPI_COMM_WORLD);
    MPI_Send(&wait_time, 1, MPI_DOUBLE, MASTER, TIME_WAIT, MPI_COMM_WORLD);
}
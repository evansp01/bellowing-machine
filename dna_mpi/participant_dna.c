#include <mpi.h>
#include <time.h>
#include <stdlib.h>
#include "constants_dna.h"

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

void worker_routine(int rank)
{
    int n, k, len;
    MPI_Recv(&n, 1, MPI_INT, MASTER, SEND_N, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&k, 1, MPI_INT, MASTER, SEND_K, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&len, 1, MPI_INT, MASTER, SEND_LEN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("Worker %d recieved instructions that dataset contains %d points of length %d and will have %d means\n", rank, n, len, k);
    char *points = (char *) malloc(len * n * sizeof(char));
    char *means = (char *) malloc(len * k * sizeof(char));
    int *mean_sums = (int *) malloc(len * k * 4 * sizeof(int));
    int *mean_counts = (int *) malloc(k * sizeof(int));
    MPI_Recv(points, len * n, MPI_CHAR, MASTER, SEND_POINTS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    while (true)
    {
        int cont;
        MPI_Recv(&cont, 1, MPI_INT, MASTER, SEND_CONTINUE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (!cont)
        {
            printf("Process %d recieved abort signal\n", rank);
            break;
        }
        memset(mean_sums, 0, DIM * k * sizeof(char));
        memset(mean_counts, 0, k * sizeof(int));
        MPI_Recv(means,  DIM * k, MPI_CHAR, MASTER, SEND_MEANS, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < n; i++)
        {
            char *point = &points[DIM * i];
            int mean_index = dna_most_similar(k, len, means, point);
            //printf("For point %d, decided closest to mean %d\n", i, mean_index);
            for (int j = 0; j < len; j++)
            {
                int charindex = 0;
                switch (point[j])
                {
                case 'T':
                    charindex = 0;
                    break;
                case 'C':
                    charindex = 1;
                    break;
                case 'A':
                    charindex = 2;
                    break;
                case 'G':
                    charindex = 3;
                    break;
                }
                mean_sums[len * 4 * mean_index + charindex] += 1;
            }
            mean_counts[mean_index] += 1;
        }
        MPI_Send(mean_sums, 4 * len * k, MPI_INT, MASTER, REPLY_MEANS, MPI_COMM_WORLD);
        MPI_Send(mean_counts, k, MPI_INT, MASTER, REPLY_COUNTS, MPI_COMM_WORLD);
    }
    free(points);
    free(means);
}
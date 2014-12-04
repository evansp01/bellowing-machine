#ifndef PARTICIPANT_   /* Include guard */
#define PARTICIPANT_

void worker_routine(int rank);
void calc_sums_counts(int k, int n, double *points, double *means,
                      double *mean_sums, int *mean_counts);

#endif
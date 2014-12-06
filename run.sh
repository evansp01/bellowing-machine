#!/bin/bash

for data in points_small points_large points_huge; do
  for numprocs in 2 4 8 12; do
    for iter in 1 10 100; do
      fname=${data}_${numprocs}_${iter}.out
      echo $fname
      mpirun -np $numprocs --hostfile ./src/ssh/hosts \
        -mca plm_rsh_no_tree_spawn 1 ./points_mpi -d ./$data.dat -i $iter \
        -o /dev/stdout -c 2 > $fname
    done
  done
done

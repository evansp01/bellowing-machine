#!/bin/bash

for size in 100000 1000000 10000000; do
    for numprocs in 2 4 8 12; do
        for iter in 1 10 100; do
            fname=points_${size}_${numprocs}_${iter}.out
            echo $fname
            mpirun -np $numprocs --hostfile ./src/ssh/hosts \
               -mca plm_rsh_no_tree_spawn 1 ./points_mpi -d ./points_${size}.dat -i $iter \
               -o /dev/stdout -c 2 > $fname
            mpirun -np $numprocs \
               -mca plm_rsh_no_tree_spawn 1 ./points_mpi -d ./points_${size}.dat -i $iter \
               -o /dev/stdout -c 2 > local_$fname
       done
   done
done

for size in 100000 1000000; do
    for numprocs in 2 4 8 12; do
        for iter in 1 10 100 1000; do
            fname=dna_${size}_${numprocs}_${iter}.out
            echo $fname
            mpirun -np $numprocs --hostfile ./src/ssh/hosts \
                -mca plm_rsh_no_tree_spawn 1 ./dna_mpi -d ./dna_${size}.dat -i $iter \
                -o /dev/stdout -c 2 > $fname
            mpirun -np $numprocs \
                -mca plm_rsh_no_tree_spawn 1 ./dna_mpi -d ./dna_${size}.dat -i $iter \
                -o /dev/stdout -c 2 > local_$fname
        done
    done
done

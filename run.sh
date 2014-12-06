#!/bin/bash

usage() {
    echo "USAGE: $0 (remote|local) (dna|points)"
    exit 1
}

case "$1" in
"remote")
    mpi_opt="--hostfile ./src/ssh/hosts"
    fprefix="";;
"local")
    mpi_opt=""
    fprefix="local_";;
*)
    usage;;
esac

case "$2" in
"points")
    size_list=(100000 1000000 10000000)
    iter_list=(1 10 100 1000)
    cluster_list=(2 100)
    type="$2";;
"dna")
    size_list=(100000 1000000)
    iter_list=(1 10 100)
    cluster_list=(2)
    type="$2";;
*)
    usage;;
esac

numprocs_list=(2 4 8 12)

for size in ${size_list[@]}; do
    for numprocs in ${numprocs_list[@]}; do
        for iter in ${iter_list[@]}; do
            for cluster in ${cluster_list[@]}; do
                dat_file=./${type}_${size}.dat
                out_file=${fprefix}${type}_${size}_${numprocs}_${iter}_${cluster}.out
                echo $out_file
                mpirun -np $numprocs $mpi_opt \
                   -mca plm_rsh_no_tree_spawn 1 ./${type}_mpi -d $dat_file -i $iter \
                   -o /dev/stdout -c $cluster > $out_file
            done
        done
    done
done

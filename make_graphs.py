#!/usr/bin/env python2

import matplotlib.pyplot as plt

data_list     = ["points_small", "points_large", "points_huge"]
numprocs_list = [2,4,8,12]
iter_list     = [1,10,100]

def filename(data, numprocs, iter):
    return "%s_%d_%d.out" % (data, numprocs, iter)

def process_output_file(data, numprocs, iter):
    with open(filename(data, numprocs, iter)) as f:
        lines = [l.split() for l in f.readlines()]
    master = lines[0]
    workers = lines[1:]
    total_time = float(master[1])
    sum_total = sum(float(w[1]) for w in workers)
    sum_wait = sum(float(w[2]) for w in workers)
    return total_time, sum_total, sum_wait

def plot_iter(times, data, numprocs):
    total = []
    sum_t = []
    sum_w = []
    for iter in iter_list:
        total.append(times[data,numprocs,iter,"total_time"])
        sum_t.append(times[data,numprocs,iter,"sum_total"])
        sum_w.append(times[data,numprocs,iter,"sum_wait"])

    plt.figure()
    plt.plot(iter_list, total, label="Total time taken")
    plt.plot(iter_list, sum_t, label="Sum of total worker times")
    plt.plot(iter_list, sum_w, label="Sum of worker idle times")
    plt.xlabel("Number of iterations")
    plt.ylabel("Time (s)")
    plt.legend()
    plt.show()

def process_all():
    times = dict()
    for data in data_list:
        for numprocs in numprocs_list:
            for iter in iter_list:
                tt, st, sw = process_output_file(data, numprocs, iter)
                times[data,numprocs,iter,"total_time"] = tt
                times[data,numprocs,iter,"sum_total"] = st
                times[data,numprocs,iter,"sum_wait"] = sw
    
    plot_iter(times)

if __name__ == '__main__':
    process_all()

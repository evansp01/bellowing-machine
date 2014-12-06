#!/usr/bin/env python2

import matplotlib.pyplot as plt

size_list     = [100000, 1000000, 10000000]
numprocs_list = [2,4,8,12]
iter_list     = [1,10,100]
cluster_list  = [2,100]

def filename(type, size, numprocs, iter):
    return "%s_%d_%d_%d.out" % (type, size, numprocs, iter)

def process_output_file(type, size, numprocs, iter):
    with open(filename(type, size, numprocs, iter)) as f:
        lines = [l.split() for l in f.readlines()]
    master = lines[0]
    workers = lines[1:]
    total_time = float(master[1])
    sum_total = sum(float(w[1]) for w in workers)
    sum_wait = sum(float(w[2]) for w in workers)
    return total_time, sum_total, sum_wait

def plot_iter(times, type, size, iter_list, numprocs):
    total = []
    sum_t = []
    sum_w = []
    useful = []
    for iter in iter_list:
        total.append(times[type,size,numprocs,iter,"total_time"])
        sum_t.append(times[type,size,numprocs,iter,"sum_total"])
        sum_w.append(times[type,size,numprocs,iter,"sum_wait"])
        useful.append(times[type,size,numprocs,iter,"useful"])

    plt.figure()
    plt.plot(iter_list, total, label="Total time taken")
    plt.plot(iter_list, useful, label="Total computing time")
    plt.plot(iter_list, sum_t, label="Sum of total worker times")
    plt.plot(iter_list, sum_w, label="Sum of worker idle times")
    plt.xlabel("Number of iterations")
    plt.ylabel("Time (s)")
    plt.legend()
    filename = "iter_%s_%d_%d.png" % (type, size, numprocs)
    plt.savefig(filename)
    plt.show()

def plot_size(times, type, size_list, iter, numprocs):
    total = []
    sum_t = []
    sum_w = []
    useful = []
    for size in size_list:
        total.append(times[type, size, numprocs, iter, "total_time"])
        sum_t.append(times[type, size, numprocs, iter, "sum_total"])
        sum_w.append(times[type, size, numprocs, iter, "sum_wait"])
        useful.append(times[type, size, numprocs, iter, "useful"])

    plt.figure()
    plt.xscale('log')
    plt.plot(size_list, total, label="Total time taken")
    plt.plot(size_list, useful, label="Total computing time")
    plt.plot(size_list, sum_t, label="Sum of total worker times")
    plt.plot(size_list, sum_w, label="Sum of worker idle times")
    plt.xlabel("Size of dataset")
    plt.ylabel("Time (s)")
    plt.legend()
    filename = "size_%s_%d_%d.png" % (type, numprocs, iter)
    plt.savefig(filename)
    plt.show()

def plot_numprocs(times, type, size, iter, numprocs_list):
    total = []
    sum_t = []
    sum_w = []
    useful = []
    for numprocs in numprocs_list:
        total.append(times[type, size, numprocs, iter, "total_time"])
        sum_t.append(times[type, size, numprocs, iter, "sum_total"])
        sum_w.append(times[type, size, numprocs, iter, "sum_wait"])
        useful.append(times[type, size, numprocs, iter, "useful"])

    plt.figure()
    plt.plot(numprocs_list, total, label="Total time taken")
    plt.plot(numprocs_list, useful, label="Total computing time")
    plt.plot(numprocs_list, sum_t, label="Sum of total worker times")
    plt.plot(numprocs_list, sum_w, label="Sum of worker idle times")
    plt.xlabel("Number of workers")
    plt.ylabel("Time (s)")
    plt.legend()
    filename = "numprocs_%s_%d_%d.png" % (type, size, iter)
    plt.savefig(filename)
    plt.show()

def plot_clusters(times, type, size, iter, numprocs):
    total = []
    sum_t = []
    sum_w = []
    useful = []
    for cluster in cluster_list:
        total.append(times[type, size, numprocs, iter, "total_time"])
        sum_t.append(times[type, size, numprocs, iter, "sum_total"])
        sum_w.append(times[type, size, numprocs, iter, "sum_wait"])
        useful.append(times[type, size, numprocs, iter, "useful"])

    plt.figure()
    plt.plot(cluster_list, total, label="Total time taken")
    plt.plot(cluster_list, useful, label="Total computing time")
    plt.plot(cluster_list, sum_t, label="Sum of total worker times")
    plt.plot(cluster_list, sum_w, label="Sum of worker idle times")
    plt.xlabel("Number of clusters")
    plt.ylabel("Time (s)")
    plt.legend()
    filename = "cluster_%s_%d_%d.png" % (type, size, iter)
    plt.savefig(filename)
    plt.show()

def process_all():
    times = dict()
    for size in size_list:
        for numprocs in numprocs_list:
            for iter in iter_list:
                tt, st, sw = process_output_file("points", size, numprocs, iter)
                times["points", size,numprocs,iter,"total_time"] = tt
                times["points", size,numprocs,iter,"sum_total"] = st
                times["points", size,numprocs,iter,"sum_wait"] = sw
                times["points", size,numprocs,iter,"useful"] = st - sw
    
    plot_iter(times, "points", 1000000, iter_list, 4)
    plot_iter(times, "points", 10000000, iter_list, 12)
    plot_size(times, "points", size_list, 100, 4)
    plot_size(times, "points", size_list, 10, 12)
    plot_numprocs(times, "points", 1000000, 100, numprocs_list)
    plot_numprocs(times, "points", 10000000, 100, numprocs_list)

if __name__ == '__main__':
    process_all()

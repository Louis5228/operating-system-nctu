#!/usr/bin/env python3

import matplotlib.pyplot as plt
import numpy as np
import math
import csv

path_list = ['multi_thread_int.csv', 'multi_process_int.csv', \
                'multi_thread_string.csv', 'multi_process_string.csv', \
                'multi_thread_struct.csv', 'multi_process_struct.csv']

for i, path in enumerate(path_list):
    X, Y, Z = [], [], [] # X:datasize, Y:thread/process num, Z:time
    with open(path, newline='') as csvfile:
        rows = csv.reader(csvfile, delimiter=',')
        # headers = next(rows)
        for row in rows:
            X.append(int(row[0]))
            Y.append(int(row[1]))
            Z.append(int(row[2]))

    plot_x, plot_y = [[] for i in range(15)], [[] for i in range(15)]
    spec_datasize_plot_x = [[] for i in range(int(math.log(100000000, 10)))]
    spec_datasize_plot_y = [[] for i in range(int(math.log(100000000, 10)))]

    ### specify thread/process ###
    for total_points in range(len(X)):
        plot_x[Y[total_points]-1].append(X[total_points])
        plot_y[Y[total_points]-1].append(Z[total_points])

        spec_datasize_plot_x[int(math.log(X[total_points], 10))-1].append(Y[total_points])
        spec_datasize_plot_y[int(math.log(X[total_points], 10))-1].append(Z[total_points])

    for thread in range(15): 
        plt.plot(plot_x[thread], plot_y[thread], '.-')

    plt.xscale("log", basex=10)
    plt.grid()

    if i % 2 == 0:
        plt.legend([str(i+1)+" thread" for i in range(15)])
    else:
        plt.legend([str(i+1)+" process" for i in range(15)])
    plt.xlabel("datasize")
    plt.ylabel("time(us)")
    plt.title(path[:-4])
    plt.savefig(path[:-4]+".png")
    plt.clf()


    ### specify thread/process ###
    legend_list = []
    for k in range(int(math.log(100000000, 10))):
        if(k >= 2) and (k < 7):
            legend_list.append("datasize=10^" + str(k))
            plt.plot(spec_datasize_plot_x[k], spec_datasize_plot_y[k], '.-')

    plt.grid()
    plt.legend(legend_list)
    
    if i % 2 == 0:
        plt.xlabel("thread")
    else:
        plt.xlabel("process")

    plt.ylabel("time(us)")
    plt.title("specify_datasize " + path[:-4])
    plt.savefig("specify_datasize " + path[:-4] + ".png")
    plt.clf()
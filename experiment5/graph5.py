import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def read_parse():
    path = "/home/yutan/cdmt/results/experiment5/"
    with open(path + "hashtime.txt", "r") as f:
        txt = f.read().rstrip(', \n')
        hashtime = txt.split(',')    
        for i in range(len(hashtime)):
            hashtime[i] = hashtime[i].split(':')
            hashtime[i][1] = int(hashtime[i][1])

    with open(path + "cdmttime.txt", 'r') as f:
        txt = f.read().rstrip(', \n')
        cdmttime = txt.split(',')
        for i in range(len(cdmttime)):
            cdmttime[i] = cdmttime[i].split(':')
            cdmttime[i][1] = int(cdmttime[i][1])

    with open(path + "size.txt", 'r') as f:
        txt = f.read().rstrip(', \n')
        size = txt.split(',')
        for i in range(len(size)):
            size[i] = size[i].split(':')
            size[i][1] = int(size[i][1])

    return hashtime, cdmttime, size

def graph(dict_):
    size = list()
    htime = list()
    ctime = list()

    for key, val in dict_.items():
        size.append(val[2])
        htime.append(val[0])
        ctime.append(val[1])
    size = np.array(size)/(1024 * 1024)
    htime = np.array(htime)/(10 ** 9)
    ctime = np.array(ctime)/(10 ** 9)
    x = np.arange(len(size)) + 1
    fig, ax = plt.subplots(figsize = (12, 7))
    plt.gcf().subplots_adjust(bottom = 0.25)
    font = {'fontsize':14, 'fontweight':'bold'}
    ax.tick_params(axis = 'both', which = 'major', labelsize = 14)
    ax.tick_params(axis = 'both', which = 'minor', labelsize = 14)

    ax.scatter(size, htime, s = 100, c = 'w', marker = 'o', edgecolor = 'black', label = "Hashing") 
    ax.scatter(size, ctime, s = 100, c = 'black', marker = 'x', edgecolor = 'black', label = "Indexing")
    properties = {'weight': 'bold'}
    ax.set_yscale('log')
    plt.legend(fontsize = 14, prop = properties)
    plt.xlabel("Size (MB)", fontsize = 18, fontweight = 'bold')
    plt.ylabel("Time (Seconds)", fontsize = 18, fontweight = 'bold')
    plt.savefig("../pics/experiment5.png", format = 'png')
         

def main():
    time = dict()
    hashtime, cdmttime, size = read_parse() 
    for each in hashtime:
        if each[0] in time:
            raise Exception("this should not happen")
        time[each[0]] = list() 
        time[each[0]].append(each[1])
    
    for each in cdmttime:
        if not each[0] in time:
            raise Exception("didnt find the match")
        time[each[0]].append(each[1])

    for each in size:
        if not each[0] in time:
            raise Exception("didnt find the match")
        time[each[0]].append(each[1])
    graph(time)
       

if __name__ == "__main__":
    main()

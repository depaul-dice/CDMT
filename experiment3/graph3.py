import os
import sys
import numpy as np
import matplotlib.pyplot as plt

def graph(files, mt_change, ct_change, mt_nodes, ct_nodes):
    mt_change = np.array(mt_change)
    ct_change = np.array(ct_change)
    mt_nodes = np.array(mt_nodes)
    ct_nodes = np.array(ct_nodes)
    mt = 1 - mt_change/mt_nodes
    ct = 1 - ct_change/ct_nodes

    x = np.arange(len(mt_change)) + 1
    fig, ax = plt.subplots(figsize = (12, 7))
    plt.gcf().subplots_adjust(bottom = 0.25)
    ax.bar(x - 0.1, mt, width = 0.2, color = 'w', edgecolor = 'black', hatch = 'x', align = 'center', label = 'MT')
    ax.bar(x + 0.1, ct, width = 0.2, color = 'black', edgecolor = 'black', align = 'center', label = 'CDMT')

    fontdict = {'fontsize': 12, 'fontweight': 'bold'}
    degree = 90
    legend_properties = {'weight': 'bold'}
    ax.set_xticks(x)
    ax.set_xticklabels(files, fontdict)
    ax.legend(fontsize = 12, prop = legend_properties)
    ax.set_yticklabels(ax.get_yticks(), fontdict)
    plt.xticks(rotation = degree)
    plt.ylabel("Ratio of Common Nodes Detected", fontsize = 18, fontweight = 'bold')
    plt.xlabel("Images", fontsize = 18, fontweight = 'bold')
    plt.savefig("../pics/experiment3.png", format = "png")

def read_parse():
    path = '/home/yutan/cdmt/results/experiment3/'
    files = list()
    mt_totalchange = list()
    ct_totalchange = list()
    mt_totalnode = list()
    ct_totalnode = list()
    for file in os.listdir(path):
        #print(file)
        mttc = 0; cttc = 0; tnm = 0; tnc = 0;
        if not os.path.isfile(path + file):
            continue
        if file[-4:] != ".txt":
            print(file)
            raise Exception("file format wrong")
        files.append(file[:-4])
        #print(path + file)
        with open(path + file, 'r') as f:
            while True:
                line = f.readline().rstrip("\n ")
                #print(line)
                if len(line) == 0:
                    break
                if file[:-4] == 'golang':
                    print(line)
                old_path, new_path, tmp1, tmp2, tmp3, tmp4 = line.split(':')
                
                mttc += int(tmp1)
                cttc += int(tmp2)
                tnm += int(tmp3)
                tnc += int(tmp4)
        
        mt_totalchange.append(mttc)
        ct_totalchange.append(cttc)
        mt_totalnode.append(tnm)
        ct_totalnode.append(tnc)

    return files, mt_totalchange, ct_totalchange, mt_totalnode, ct_totalnode

def main():
    files, mt_totalchange, ct_totalchange, mt_totalnode, ct_totalnode = read_parse()
    graph(files, mt_totalchange, ct_totalchange, mt_totalnode, ct_totalnode)

if __name__ == "__main__":
    main()

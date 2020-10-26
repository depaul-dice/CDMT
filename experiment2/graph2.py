import os
import pandas as pd
import numpy as np
from experiment2 import read_images
import matplotlib.pyplot as plt

def graph(images, dedup_rates, dedup_sizes, gz_rates, gz_sizes): 
    plt.clf()
    legend_properties = {'weight': 'bold'}
    fig, ax = plt.subplots(figsize = (12, 7))
    x = np.arange(len(dedup_rates)) + 1
    bd = ax.bar(x - 0.1, dedup_rates, width = 0.1, color = 'w', align='center', edgecolor = 'black', label = 'Deduplication')
    bg = ax.bar(x, gz_rates, width = 0.1, color = 'w', hatch = '/', align='center', edgecolor = 'black', label = 'Compression')
    bo = ax.bar(x + 0.1, np.ones(len(x)), width = 0.1, color = 'black', align='center', edgecolor = 'black', label = 'Actual')
    
    fontdict = {'fontsize': 14, 'fontweight':'bold'}

    plt.gcf().subplots_adjust(bottom = 0.25)
    plt.xticks(rotation = 30)
    ax.set_xticks(x)
    ax.set_xticklabels(images, fontdict)
    ax.legend(fontsize = 14, prop=legend_properties)

    ax.set_yticklabels(ax.get_yticks(), fontdict)
    plt.xlabel("Images", fontsize = 20, fontweight = 'bold')
    plt.ylabel("Compression Ratio", fontsize = 20, fontweight='bold')
    plt.savefig("../pics/experiment2.png", format = "png")

def read_parse():
    image_file = "../data.txt"
    images = read_images(image_file)
    directory = "/home/yutan/cdmt/results/experiment2/"
    images_rv = list()
    dedup_rates = list()
    dedup_sizes = list()
    gz_rates = list()
    gz_sizes = list()
   
    for image in images:
        path = directory + image + '.txt'
        with open(path, 'r') as f:
            while True:
                line = f.readline().rstrip("\n ")
                if len(line) == 0:
                    break
                image_rv, dedup_rate, dedup_size, gz_rate, gz_size = line.split(':')
                images_rv.append(image.capitalize())
                dedup_rates.append(dedup_rate)
                dedup_sizes.append(dedup_size)
                gz_rates.append(gz_rate)
                gz_sizes.append(gz_size)
                
    with open(directory + 'global.txt') as f:
        while True:
            line = f.readline().rstrip("\n ")
            if len(line) == 0:
                break
            image_rv, dedup_rate, dedup_size, gz_rate, gz_size = line.split(':')
            images_rv.append(image_rv.capitalize())
            dedup_rates.append(dedup_rate)
            dedup_sizes.append(dedup_size)
            gz_rates.append(gz_rate)
            gz_sizes.append(gz_size)
        
    print(dedup_rates)
    print(gz_rates)

    return images_rv, dedup_rates, dedup_sizes, gz_rates, gz_sizes 

def reciprocal(dedup_rates, gz_rates):
    assert len(dedup_rates) == len(gz_rates)
    for i in range(len(dedup_rates)):
        dedup_rates[i] = 1/float(dedup_rates[i])
        gz_rates[i] = 1/float(gz_rates[i])
    return dedup_rates, gz_rates

def format_sizes(dedup_sizes, gz_sizes):
    assert len(dedup_sizes) == len(gz_sizes)
    for i in range(len(dedup_sizes)):
        dedup_sizes = int(dedup_sizes)
        gz_sizes = int(gz_sizes)
        
def main():
    image_file = "../data.txt"
    images = read_images(image_file)

    images, dedup_rates, dedup_sizes, gz_rates, gz_sizes = read_parse()
    dedup_rates, gz_rates = reciprocal(dedup_rates, gz_rates)
    graph(images, dedup_rates, dedup_sizes, gz_rates, gz_sizes)

if __name__ == "__main__":
    main()

import numpy as np
import matplotlib.pyplot as plt

def read_parse():
    path = "/home/yutan/cdmt/results/experiment2/global.txt"  
    img, dr, ds, gr, gs = ([] for i in range(5))
    with open(path, 'r') as f:
        while True:
            line = f.readline().rstrip('\n ')
            if line == '':
                break
            image, dedup_rate, dedup_size, gz_rate, gz_size = line.split(':')
            image = image[image.rfind('/') + 1:]
            img.append(image)
            dr.append(1 - float(dedup_rate))
            ds.append(dedup_size)
            gr.append(float(gz_rate))
            gs.append(gz_size)
    return img, dr, ds, gr, gs

def graph(img, dr, gr):
    fontdict = {'fontsize': 14, 
            'fontweight':'bold',
            'verticalalignment': 'baseline'
            }
    legend_properties = {'weight': 'bold'}
    degree = 90
    #plt.gcf().subplots_adjust(bottom = 0.25)
    x = np.arange(len(img)) + 1
    fig, ax = plt.subplots(figsize = (12, 7))
    ax.plot(x, dr, 'o-', label = 'deduplication');
    ax.plot(x, gr, 'x-', label = 'compression')
    ax.set_xticks(x)
    ax.set_xticklabels(img, fontdict)
    ax.legend(fontsize = 14, prop = legend_properties)
    plt.xticks(rotation = degree)
    plt.xlabel("Images", fontsize = 14, fontweight = 'bold')
    plt.ylabel("Global Compression Ratio", fontsize = 14, fontweight='bold')
    plt.savefig('../pics/experiment2-2.png', format = 'png')
    
def main():
    img, dr, ds, gr, gs = read_parse() 
    graph(img, dr, gr)

if __name__=="__main__":
    main()

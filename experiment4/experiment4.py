import pandas as pd
import numpy as np 
import matplotlib.pyplot as plt
import sys
import os
import subprocess

'''
info that is necessary
1. how many nodes were compared
2. how many leaves are there
3. filenames
4. size
'''

def main():
    images = ['deepmind', 'django', 'mysql', 'nginx', 'postgres', 'pytorch', 'golang', 'node', 'tomcat', 'tensorflow', 'r-base', 'python', 'redis', 'rails']
    for image in images:
        run_main(image)
        old_paths, new_paths, leaves, node_compared, dedup_ratio = parse()
        
        assert len(old_paths) == len(new_paths) and len(leaves) == len(node_compared) and len(leaves) == len(dedup_ratio) and len(old_paths) == len(leaves)
        if len(old_paths) > 0:

            with open('/home/yutan/cdmt/results/experiment4/' + image + '.txt', 'w') as f:
                for i in range(len(old_paths)):
                    f.write(image)
                    f.write(',')
                    f.write(old_paths[i])
                    f.write(',')
                    f.write(new_paths[i])
                    f.write(',')
                    f.write(str(leaves[i]))
                    f.write(',')
                    f.write(str(node_compared[i]))
                    f.write(',')
                    f.write(str(dedup_ratio[i]))
                    f.write('\n')
                
    if os.path.exists('nodescompared.csv'):
        os.remove('nodescompared.csv')

    #dedup_ratios = get_dedup_ratios(old_paths, new_paths, images)
    '''
    f = open("4result.txt", "r")
    line = f.readlines(1)
    dedup_ratios = line[0].split(',')
    dedup_ratios.pop()
    for i in range(len(dedup_ratios)):
        dedup_ratios[i] = float(dedup_ratios[i])
    f.close()
    '''

    #graph(new_paths, leaves, node_compared, dedup_ratios, images)

def run_main(image):
    if os.path.exists('nodescompared.csv'):
        os.remove('nodescompared.csv')
    order = "cdmt4/main " + image
    os.system(order)

def parse():
    if os.path.exists("nodescompared.csv"):
        df = pd.read_csv("nodescompared.csv", header = None)
    else:
        print("nodescompared.csv did not exist, skipping")
        return list(), list(), list(), list(), list()
    values = df.values
    old_paths = list()
    new_paths = list()
    leaves = list()
    node_compared = list()
    total_nodes = list()
    nodes_changed = list()
    for value in values[0]:
        if isinstance(value, str):
            value = value.split(':')
            count = 0
            for each in value:
                if count == 0:
                    old_paths.append(each)
                    count += 1
                elif count == 1:
                    new_paths.append(each)
                    count += 1
                elif count == 2:
                    leaves.append(int(each))
                    count += 1
                elif count == 3:
                    node_compared.append(int(each))
                    count += 1
                elif count == 4:
                    total_nodes.append(int(each))
                    count += 1
                elif count == 5:
                    nodes_changed.append(int(each))
                    count += 1
                    break
                    
    total_nodes = np.array(total_nodes)
    nodes_changed = np.array(nodes_changed)
    dedup_ratio = 1 - nodes_changed/total_nodes
    return old_paths, new_paths, leaves, node_compared, dedup_ratio

def get_sizes(images):
    rv = list()
    print("getting sizes")
    for directory in images:
        #directory = sys.argv[1]
        for filename in os.listdir(directory):
            print(filename)

            process = subprocess.Popen(['stat', '-c', '%s', directory + '/' + filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = process.communicate()
            rv.append((filename[:-4], int(out)))
            
    return rv

#def graph(new_paths, leaves, compared, dedup_ratios, images):

    images = ['Golang', 'Node', 'Tomcat', 'Tensorflow', 'R-Base', 'Python', 'Redis', 'Rails']
    markers = ['o', 'v', '^', 'h', 'D', 'P', 'X', 's']
    compared = np.array(compared)
    leaves = np.array(leaves)
    count = 0
    curr_image = new_paths[0][:new_paths[0].rfind('/')]
    curr_image = curr_image[curr_image.rfind('/') + 1:]
    prev_image = ''
    img_cnt = 0
    i = 0
    fig, ax = plt.subplots(figsize = (12, 7))
    while i < len(dedup_ratios):
        x = list()
        y = list()
        while i < len(dedup_ratios) and (count == 0 or prev_image == curr_image):
            x.append(dedup_ratios[i])
            y.append(compared[i]/leaves[i])

            prev_image = curr_image
            count += 1
            i += 1
            try:
                curr_image = new_paths[i][:new_paths[i].rfind('/')]
                curr_image = curr_image[curr_image.rfind('/') + 1:]
            except:
                break
            
        ax.scatter(x, y, s = 100, c = 'w', edgecolor = 'black', label = images[img_cnt], marker = markers[img_cnt]) 
        img_cnt += 1
        count = 0 

    
    plt.gcf().subplots_adjust(bottom = 0.25)
    font = {'fontsize':14,
            'fontweight': 'bold'}
    properties = {'weight':'bold'} 
    yticks = ax.get_yticks()
    xticks = ax.get_xticks()
    for i in range(len(yticks)):
        xticks[i] = round(xticks[i] + 0.1, 1) * 2
        yticks[i] = round(yticks[i], 1)
    ax.set_yticklabels(yticks, font)
    ax.set_xticklabels(xticks, font)

    plt.legend(fontsize = 14, prop=properties)
    plt.xlabel("Deduplication Ratio", fontsize = 18, fontweight = 'bold')
    plt.xlim((0, 1))
    plt.ylabel("Comparison Ratio", fontsize = 18, fontweight = 'bold')
    plt.savefig('4vsdedup.png', format = 'png')

if __name__ == "__main__":
    main() 
    

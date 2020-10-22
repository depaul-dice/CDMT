import pandas as pd
import numpy as np
import os
import sys
import subprocess
import matplotlib.pyplot as plt

def getsize(filename):
    process = subprocess.Popen(['stat', '-c', '%s', filename], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = process.communicate()
    try:
        out = int(out)
    except:
        print(filename)
        raise ValueError
    return out

def targz_iterations_noorder(image):
    rates = list()
    sizes = list()
    global_size = 0
    global_reduced = 0
    for tag in os.listdir(image):
        directory = image + "/" + tag
        reduced = 0
        normal = 0
        print("tag: " + tag)
        for layer in os.listdir(directory):
            print("layer: " + layer)
            filename = directory + '/' + layer 
            order = "tar -zcvf " + filename + ".gz " + filename
            os.system(order)
            normal += getsize(filename)
            reduced += getsize(filename + ".gz")
            os.remove(filename + ".gz")
        global_size += normal
        global_reduced += reduced
    rates.append(global_reduced/global_size)
    sizes.append(global_size)
    return rates, sizes 

def dedup_iterations_noorder(image):
    global_dedup = {}
    global_saved = 0
    global_size = 0
    rates = list()
    sizes = list()
    tags = list()
    for tag in os.listdir(image):
        dedup = {}
        print("tag: " + tag)
        directory = image + "/" + tag
        for layer in os.listdir(directory):
            print("layer: " + layer)
            # deduping here for an layer
            order = "/home/yutan/cdmt/vvpkg/demo/dump_blocks " + directory + "/" + layer + " > out 2> err"
            os.system(order)

            df1 = pd.read_csv("out", header = None, sep=']');
            df2 = pd.read_csv("err", header = None);
            df1 = df1.values[0]
            df2 = df2.values[0]
            
            for i in range(len(df1)):
                if isinstance(df1[i], str):
                    sha = df1[i][df1[i].find("\""):df1[i].rfind('\"') + 1]
                    if sha in global_dedup:
                        global_saved += df2[i] 
                    else:
                        global_dedup[sha] = df2[i]
                    print(df2[i])
                    global_size += df2[i]
                        
    rates.append(1 - global_saved/global_size)
    sizes.append(global_size)
    tags.append("global")
    os.remove("out")
    os.remove("err")
    #f.close()

    return rates, sizes, tags
           
def main(rawimage:str):
    prefix = "/home/yutan/cdmt/data/"
    image = prefix + rawimage
    
    dedup_rates, dedup_sizes, filenames = dedup_iterations_noorder(image)
    gz_rates, gz_sizes = targz_iterations_noorder(image)
    assert len(dedup_rates) == len(filenames) and len(filenames) == len(gz_rates)
    assert len(dedup_rates) == len(dedup_sizes) and len(gz_rates) == len(gz_sizes)

    with open('/home/yutan/cdmt/results/experiment2/'+rawimage+'.txt', 'w') as f:
        for i in range(len(filenames)):
            f.write(filenames[i])
            f.write(':')
            f.write(str(dedup_rates[i]))
            f.write(':')
            f.write(str(dedup_sizes[i]))
            f.write(':')
            f.write(str(gz_rates[i]))
            f.write(':')
            f.write(str(gz_sizes[i]))
            f.write('\n')

    '''
    print("done targz and dedup")
    print("filenames: ", end = ",")
    print(filenames)
    print("dedup_rates: ", end = "")
    print(dedup_rates)
    dedup_sizes = np.array(dedup_sizes)

    fig, ax = plt.subplots(figsize = (12, 7))
    x = np.arange(len(dedup_sizes)) + 1
    bd = ax.bar(x - 0.1, dedup_rates, width = 0.2, color = 'b', align='center', label='deduplication')
    bg = ax.bar(x + 0.1, gz_rates, width = 0.2, color = 'r', align='center', label='compression')
    for i in range(len(bd)):
        yval = max(bd[i].get_height(), bg[i].get_height())
        ax.text(bd[i].get_x(), yval + 0.006, dedup_sizes[i]/1000000)
    filenames.insert(0, "dummy")
    ax.set_xticklabels(filenames)
    plt.savefig("../pics/2.png", format ="png")
    '''
    return
   
def read_images(filename):
    rv = list()
    with open(filename) as f:
        while True:
            line = f.readline().rstrip("\n ")
            if line == "":
                break
            rv.append(line)
    return rv


if __name__ == '__main__':
    if len(sys.argv) == 2:
        rawimage = sys.argv[1]
        main(rawimage)
    else:
        filename = "/home/yutan/cdmt/data.txt"
        images = read_images(filename)
        for image in images:
            main(image)


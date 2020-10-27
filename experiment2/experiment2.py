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

def read_order(prefix, image: str) -> list:
    rv = list()
    prefix = "/home/yutan/cdmt/order/"
    with open(prefix + image + '.txt', 'r') as f:
        while True:
            line = f.readline().rstrip('\n ')
            if len(line) == 0:
                break
            if line[-4:] != '.tar':
                raise Exception('file format not correct')
            line = line[:-4]
            rv.append(line)
    return rv

                
def targz_iterations_global(images: list, prefix: str):
    rates = list()
    sizes = list()
    global_size = 0
    global_reduced = 0
    for image in images:
        image = prefix + image
        for tag in os.listdir(image):
            directory = image + "/" + tag
            reduced = 0
            normal = 0
            print("tag: " + tag)
            order = "tar -zcvf " + directory + ".gz " + directory
            os.system(order)
            normal = getsize(directory)
            reduced = getsize(directory + ".gz")
            os.remove(filename + ".gz")
            rates.append(reduced/normal)
            sizes.append(normal)
            global_size += normal
            global_reduced += reduced
    rates.append(global_reduced/global_size)
    sizes.append(global_size)
    return rates, sizes 

def targz_iterations(prefix, image):
    rates = list()
    sizes = list()
    global_size = 0
    global_reduced = 0
    
    tags = read_order(prefix, image)
    for tag in tags:
        directory = prefix + image + "/" + tag
        reduced = 0
        normal = 0
        print("tag: " + tag)
        file = directory + '.tar'
        order = "tar -zcvf " + file + ".gz " + file 
        print(order)
        os.system(order)
        normal = getsize(file)
        reduced = getsize(file + ".gz")
        os.remove(file + ".gz")
        rates.append(reduced/normal)
        sizes.append(normal)
        global_size += normal
        global_reduced += reduced
    rates.append(global_reduced/global_size)
    sizes.append(global_size)
    return rates, sizes 


def dedup_iterations_global(images:list, prefix:str):
    global_dedup = {}
    global_saved = 0
    global_size = 0
    rates = list()
    sizes = list()
    tags = list()
   
    for image in images:
        image = prefix + image
        for tag in os.listdir(image):
            saved = 0
            size = 0
            print("tag: " + tag)
            directory = image + '/' + tag
            order = "/home/yutan/cdmt/vvpkg/demo/dump_blocks " + directory + " > out 2> err"
            os.system(order)

            df1 = pd.read_csv("out", header = None, sep=']');
            df2 = pd.read_csv("err", header = None);
            df1 = df1.values[0]
            df2 = df2.values[0]
            
            for i in range(len(df1)):
                if isinstance(df1[i], str):
                    sha = df1[i][df1[i].find("\""):df1[i].rfind('\"') + 1]
                    if sha in global_dedup:
                        saved += df2[i]
                        global_saved += df2[i] 
                    else:
                        global_dedup[sha] = df2[i]
                    size += df2[i]
                    global_size += df2[i]
            rates.append(1 - saved/size)
            sizes.append(size)
            tags.append(tag)

    rates.append(1 - global_saved/global_size)
    sizes.append(global_size)
    tags.append("global")
    os.remove("out")
    os.remove("err")
    #f.close()

    return rates, sizes, tags

def dedup_iterations(prefix, image):
    global_dedup = {}
    global_saved = 0
    global_size = 0
    rates = list()
    sizes = list()
    tags = list()
    iterate = read_order(prefix, image)
    for tag in iterate:
        saved = 0
        size = 0
        dedup = {}
        print("tag: " + tag)
        directory = prefix + image + "/" + tag
        # deduping here for an layer
        order = "/home/yutan/cdmt/vvpkg/demo/dump_blocks " + directory + ".tar > out 2> err"
        os.system(order)

        df1 = pd.read_csv("out", header = None, sep=']');
        df2 = pd.read_csv("err", header = None);
        df1 = df1.values[0]
        df2 = df2.values[0]
        
        for i in range(len(df1)):
            if isinstance(df1[i], str):
                sha = df1[i][df1[i].find("\""):df1[i].rfind('\"') + 1]
                if sha in global_dedup:
                    saved += int(df2[i])
                    global_saved += int(df2[i])
                else:
                    global_dedup[sha] = int(df2[i])
                size += int(df2[i])
                global_size += int(df2[i])
        rates.append(1 - saved/size)
        sizes.append(size)
        tags.append(tag)
    rates.append(1 - global_saved/global_size)
    sizes.append(global_size)
    tags.append("global")
    os.remove("out")
    os.remove("err")
    #f.close()

    return rates, sizes, tags
          
def main(rawimage:str):
    prefix = "/home/yutan/cdmt/data/"
    print("image: " + rawimage)
    if rawimage == "global":
        filename = "/home/yutan/cdmt/data.txt"
        images = read_images(filename)
        dedup_rates, dedup_sizes, filenames = dedup_iterations_global(images, prefix)
        gz_rates, gz_sizes = targz_iterations_global(images, prefix)

    else: 
        dedup_rates, dedup_sizes, filenames = dedup_iterations(prefix, rawimage)
        gz_rates, gz_sizes = targz_iterations(prefix, rawimage)

    assert len(dedup_rates) == len(filenames) and len(filenames) == len(gz_rates)

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


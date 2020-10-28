import os
import sys
import subprocess
import pandas as pd
import numpy as np
import logging

def vvpkg(out, err, pool):
    df1 = pd.read_csv(out, header = None, sep =']')
    df2 = pd.read_csv(err, header = None)
    df1 = df1.values[0]
    df2 = df2.values[0]
    hashes = list()
    offsets = [0]
    sizes = list()
    match = dict() 
    for j in range(len(df1)):
        if isinstance(df1[j], str):
            sha = df1[j][df1[j].find("\""):df1[j].rfind('\"') + 1]
            hashes.append(sha)
            sizes.append(int(df2[j]))
            offsets.append(offsets[j] + int(df2[j]))
            if sha in pool:
                assert pool[sha] == int(df2[j])
                match[sha] = int(df2[j])
            else:
                pool[sha] = int(df2[j])

    offsets.pop()
    return hashes, sizes, offsets, pool, match  

def inspect_tar(out):
    '''
    order = "bash offset.sh " + tarfile + " > out 2> err"
    os.system(order)
    '''
    files, offsets = ([] for i in range(2))
    files.append('header')
    offsets.append(0)
    with open(out, 'r') as f:
        while True:
            line = f.readline()
            if line == '':
                break
            offset, size, file = line.split()
            offsets.append(int(offset))
            files.append(file)
    return files, offsets

def offset_match(hashes, hoffsets, files, toffsets):
    rv = list()
    curr_hash = ''
    curr_hoffset = 0
    while len(hashes) > 0 and len(files) > 0:
        if hoffsets[0] < toffsets[0]:
            curr_hash = hashes.pop(0)
            curr_hoffset = hoffsets.pop(0)
            rv[len(rv) - 1][1].append((curr_hash, curr_hoffset))
        elif toffsets[0] < hoffsets[0]:
            curr_file = files.pop(0)
            curr_toffset = toffsets.pop(0)
            rv.append(((curr_file, curr_toffset), list()))
            #adding the block before
            rv[len(rv) - 1][1].append((curr_hash, curr_hoffset))
        elif hoffsets[0] == toffsets[0]:
            curr_file = files.pop(0)
            curr_toffset = toffsets.pop(0)
            curr_hash = hashes.pop(0)
            curr_hoffset = hoffsets.pop(0)
            rv.append(((curr_file, curr_toffset), list()))
            rv[len(rv) - 1][1].append((curr_hash, curr_hoffset))
        else:
            raise Exception('this path should not happen')

    if len(hashes) == 0:
        while len(files) > 0:
            curr_file = files.pop(0)
            curr_toffset = toffsets.pop(0)
            rv.append(((curr_file, curr_toffset), list()))
            rv[len(rv) - 1][1].append((curr_hash, curr_hoffset))
    elif len(files) == 0:
        while len(hashes) > 0:
            curr_hash = hashes.pop(0)
            curr_hoffset =  hoffsets.pop(0)
            rv[len(rv) - 1][1].append((curr_hash, curr_hoffset))
    else:
        raise Exception('this path should not happen')
    return rv

def layer_match(layers: list, match: dict) -> int:
    
    size = 0
    print(match)
    for item in layers:
        layername = item[0][0] 
        if layername[-4:] != '.tar':
            continue
        localsize = 0
        cnt = 0
        for hash in item[1]:
            if hash[0] in match:
                localsize += hash[1] 
                print(cnt)
            else:
                print(hash[0])
                print('no good: %d'%cnt)
                localsize = 0 
                break
        size += localsize
        
    return size 


def iteration(image, tag, pool):
    prefix = '/home/yutan/cdmt/'
    out = prefix + 'tmp/' + image + '/' + tag + '.tar.bk'
    err = prefix + 'sizes/' + image + '/' + tag + '.tar.sz'
    file = prefix + 'taroffset/' + image + '/' + tag + '.tar.of'

    hashes, hsizes, hoffsets, pool, match = vvpkg(out, err, pool)
    assert len(hashes) == len(hoffsets) and len(hashes) == len(hsizes)

    files, toffsets = inspect_tar(file)
    assert len(files) == len(toffsets)

    total_size = sum(hsizes)
    layers = offset_match(hashes, hoffsets, files, toffsets)
    match_size = layer_match(layers, match)
    if match_size > 0:
        print(image)
        print(tag)
    return match_size, total_size 

def read_images(filename):
    rv = list()
    with open(filename) as f:
        while True:
            line = f.readline().rstrip("\n ")
            if line == '':
                break
            rv.append(line)
    return rv

def read_order(image):
    rv = list()
    prefix = "/home/yutan/cdmt/order/"
    with open(prefix + image + '.txt', 'r') as f:
        while True: 
            line = f.readline().rstrip('\n ')
            if line == '':
                break
            if line[-4:] != '.tar':
                raise Exception('file format not correct')
            line = line[:-4]
            rv.append(line)
    return rv

def main():
    filename = '/home/yutan/cdmt/data.txt'
    images = read_images(filename)
    os.remove('/home/yutan/cdmt/results/experiment7/table2.txt')
    for image in images:
        print(image)
        pool = dict()
        tags = read_order(image) 
        agg_match, agg_size = 0, 0
        for tag in tags:
            print(tag)
            match, size = iteration(image, tag, pool) 
            agg_match += match
            agg_size += size
        with open('/home/yutan/cdmt/results/experiment7/table2.txt', 'a+') as f:
            f.write(image)
            f.write(',')
            f.write(str(agg_match/agg_size))
            f.write('\n')


if __name__ == "__main__":
    main()

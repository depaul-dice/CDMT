import os
import sys
import subprocess
import pandas as pd
import numpy as np
import logging

def vvpkg(tarfile):
    order = "/home/yutan/cdmt/vvpkg/demo/dump_blocks " + tarfile + " > out 2> err"
    os.system(order)
    df1 = pd.read_csv("out", header = None, sep =']')
    df2 = pd.read_csv("err", header = None)
    df1 = df1.values[0]
    df2 = df2.values[0]
    hashes = list()
    offsets = [0]
    for j in range(len(df1)):
        if isinstance(df1[j], str):
            sha = df1[j][df1[j].find("\""):df1[j].rfind('\"') + 1]
            hashes.append(sha)
            offsets.append(offsets[j] + int(df2[j]))

    os.remove('out')
    os.remove('err')
    offsets.pop()
    return hashes, offsets

def inspect_tar(tarfile):
    order = "bash offset.sh " + tarfile + " > out 2> err"
    os.system(order)
    files, offsets = ([] for i in range(2))
    files.append('header')
    offsets.append(0)
    with open('out', 'r') as f:
        while True:
            line = f.readline()
            if line == '':
                break
            offset, size, file = line.split()
            offsets.append(int(offset))
            files.append(file)
    os.remove('out')
    os.remove('err')
    return files, offsets

def offset_match(hashes, hoffsets, files, toffsets):
    '''
    print(hoffsets)
    print(toffsets)
    print(files)
    '''
    rv = list()
    j = 0
   
    for i in range(len(toffsets)):
        print(i)
        while hoffsets[j] < toffsets[i]: 
            '''
            print(i, end = ':'); print(j)
            print('came option 1')
            print("hoffsets[j] : %d"%hoffsets[j])
            print("toffsets[i] : %d"%toffsets[i])
            '''
            
            rv[len(rv) - 1][1].append((hashes[j], hoffsets[j]))
            j += 1
             
            if j == len(hoffsets):
                while i < len(files):
                    rv.append((files[i], list()))
                    rv[len(rv) - 1][1].append((hashes[j - 1], hoffsets[j - 1]))
                    i += 1
                return rv 

        if hoffsets[j] > toffsets[i]:
            '''
            print(rv)
            print(i, end = ':'); print(j)
            print('came option 2')
            '''
            rv.append((files[i], list()))
            if j > 0:
                rv[len(rv) - 1][1].append((hashes[j - 1], hoffsets[j - 1]))
            rv[len(rv) - 1][1].append((hashes[j], hoffsets[j]))
            j += 1
            if j == len(hoffsets):
                i += 1
                while i < len(files):
                    rv.append((files[i], list()))
                    rv[len(rv) - 1][1].append((hashes[j - 1], hoffsets[j - 1]))
                    i += 1
                return rv 
            
        elif hoffsets[j] == toffsets[i]:
            '''
            print(i, end = ':'); print(j)
            print('came option 3')
            '''
            rv.append((files[i], list()))
            rv[len(rv) - 1][1].append((hashes[j], hoffsets[j]))
            j += 1
            if j == len(hoffsets):
                # i need rest of the files to be in the same hash 
                while i < len(files):
                    rv.append((files[i], list()))
                    rv[len(rv) - 1][1].append((hashes[j - 1], hoffsets[j - 1]))
                    i += 1
                return rv 

def main(tarfile):
    if os.path.exists('out'):
        os.remove('out')
    if os.path.exists('err'):
        os.remove('err')
    hashes, hoffsets = vvpkg(tarfile)
    assert len(hashes) == len(hoffsets)
    files, toffsets = inspect_tar(tarfile)
    assert len(files) == len(toffsets)
    rv = offset_match(hashes, hoffsets, files, toffsets)
    print(rv)

if __name__ == "__main__":
    tarfile = sys.argv[1]
    main(tarfile)
